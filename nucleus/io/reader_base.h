/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef THIRD_PARTY_NUCLEUS_IO_READER_BASE_H_
#define THIRD_PARTY_NUCLEUS_IO_READER_BASE_H_

#include <algorithm>
#include <memory>

#include "absl/synchronization/mutex.h"
#include "nucleus/util/proto_ptr.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/platform/logging.h"


namespace nucleus {

// The classes declared in this file support the functionality of a
// "reader" class that allows iteration over records by a single
// iterator at once.

// IterableBase and Reader are two base classes that are entwined as follows:
//  - IterableBase has a reference to a reader, so that we can notify
//    the reader when the iterable is destructed, enabling another
//    iteration to happen.
//  - Reader has a reference to the single live iterable, enabling it
//    send a notification if the Reader is destructed before the
//    iterable is.  This is important for use from Python, where we don't
//    control the lifetimes of objects.

class IterableBase;  // Forward declaration.

class Reader {
 private:
  // Weak reference to live extant iterable, or null
  mutable IterableBase* live_iterable_ = nullptr;
  // Mutex protecting live_iterable_count.
  mutable absl::Mutex mutex_;

 protected:
  // Construct a new Iterable object, *if* we can guarantee that there
  // are no other extant Iterable objects for this Reader.  If there
  // is another extant Iterable, return nullptr.  This method should
  // be used by subclasses to provide their own methods exposing
  // Iterables.
  template<class Iterable, typename... Args>
  std::shared_ptr<Iterable> MakeIterable(Args&&... args) const {
    absl::MutexLock lock(&mutex_);
    if (live_iterable_ != nullptr) {
      LOG(WARNING) << "Returning null from MakeIterable because there's "
                   " already an active iterator";
      return nullptr;
    }
    Iterable* it =  new Iterable(std::forward<Args>(args)...);
    live_iterable_ = it;
    return std::shared_ptr<Iterable>(it);
  }

 public:
  virtual ~Reader();

  friend class IterableBase;
};


class IterableBase {
 protected:
  const Reader* reader_;

  explicit IterableBase(const Reader* reader);

 public:
  // On destruction, release the reader to be iterated again.
  virtual ~IterableBase();

  // Method to *explicitly* "release" this iterable to enable another
  // iteration to proceed. Returns OK status if the release was successful, or
  // an error if not.
  tensorflow::Status Release();

  // Is this iterable alive, in the sense that
  //  - its reader is still open; and
  //  - it has not been released?
  bool IsAlive() const;

  // Iterable implementations should indicate a not-OK status in Next() on
  // attempts to iterate on an iterable that is not live. They can call this
  // method to do the check and return the status if not OK. The standard usage
  // is: TF_RETURN_IF_ERROR(CheckIsAlive()) in Next().
  tensorflow::Status CheckIsAlive() const;

  // Python Context manager support---will be wrapped as __enter__ /
  // __exit__. This two functions should return an OK status if the enter/exit
  // were successful, or an error if not.
  tensorflow::Status PythonEnter();
  tensorflow::Status PythonExit();

  friend class Reader;
};



// This is the base class that client code should extend.
template<class Record>
class Iterable : public IterableBase {
 protected:
  explicit Iterable(const Reader* reader)
      : IterableBase(reader)
  {}

 private:
  Record current_record_;
  tensorflow::Status current_status_ = tensorflow::Status::OK();
  bool IsOK() { return current_status_.ok(); }

 public:
  // Abstract interface.

  // Next gets the next record.
  // Returns:
  //  true if we successfully got the record (is put in *out);
  //  false if there are no more records.
  virtual StatusOr<bool> Next(Record* record) = 0;

  // PythonNext is the same as Next, except the Record is wrapped
  // with a ProtoPtr<> template to avoid CLIF copies when being
  // called from Python.
  StatusOr<bool> PythonNext(ProtoPtr<Record> p) {
    return Next(p.p_);
  }

 public:
  // C++ const iterator class.
  class iterator : public std::iterator<std::input_iterator_tag, Record> {
   private:
    std::shared_ptr<Iterable> iterable_;
    bool past_end_;

   public:
    // Construct an iterator that is either...
    // 1) end=false => ...looking at the first record available in the reader
    // 2) end=true  => ...a sentinel that is "past the end"
    explicit iterator(std::shared_ptr<Iterable> iterable, bool end = false)
        : iterable_(iterable) {
      if (!end) {
        past_end_ = false; ++(*this);
      } else {
        past_end_ = true;
      }
    }

    // Get a "past-the-end" sentinel iterator.
    static iterator end(std::shared_ptr<Iterable> iterable) {
      return iterator(iterable, true);
    }

    iterator& operator++() {
      if (!past_end_ && iterable_->IsOK()) {
        StatusOr<bool> statusor = iterable_->Next(&iterable_->current_record_);
        if (statusor.ok()) {
          bool advanced = statusor.ValueOrDie();
          past_end_ = !advanced;
        } else {
          iterable_->current_status_ = statusor.status();
        }
      } else {
        past_end_ = true;
      }
      return *this;
    }

    iterator operator++(int) {
      iterator retval = *this; ++(*this); return retval;
    }

    // Note that equality for input iterators needs not be correct for
    // comparing iterators neither of which is "past-end".
    bool operator==(iterator other) const {
      return (iterable_ == other.iterable_) && (past_end_ == other.past_end_);
    }

    bool operator!=(iterator other) const { return !(*this == other); }

    const StatusOr<Record*> operator*() {
      if (!iterable_->IsOK()) {
        // Something went wrong in the last call to Next(), return its status.
        return iterable_->current_status_;
      } else if (past_end_) {
        // We've attempted to read past the end of the iterator.
        return tensorflow::errors::OutOfRange("iterator past_end_");
      } else {
        // Normal case: everything is fine, so return the current record.
        return &iterable_->current_record_;
      }
    }
  };


 public:
  // Disable copy.
  Iterable(const Iterable&) = delete;
  Iterable& operator=(const Iterable&) = delete;
  // Enable move.
  Iterable(Iterable&&) = default;
  Iterable& operator=(Iterable&&) = default;

  // begin/end for C++ iterator interface.

  // Template parameter A represents the actual Iterable subtype; we
  // need to specialize here because there is no input argument
  // covariance for shared_ptr.
  template <class A>
  friend iterator begin(std::shared_ptr<A> iterable) {
    // Note it would be better to return a Status or StatusOr here but it seems
    // that this is incompatible with iterator interface.
    CHECK(iterable)
        << "Invalid Iterable---attempt to iterate multiple times concurrently?";
    return iterator(iterable, false);
  }

  template<class A>
  friend iterator end(std::shared_ptr<A> iterable) {
    // Note it would be better to return a Status or StatusOr here but it seems
    // that this is incompatible with iterator interface.
    CHECK(iterable)
        << "Invalid Iterable---attempt to iterate multiple times concurrently?";
    return iterator::end(iterable);
  }
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_READER_BASE_H_
