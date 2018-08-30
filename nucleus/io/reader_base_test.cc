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

#include "nucleus/io/reader_base.h"

#include <algorithm>
#include <string>
#include <vector>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/platform/types.h"
#include "nucleus/vendor/status_matchers.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"

namespace tf = tensorflow;

using ::testing::Pointee;
using ::testing::StrEq;

namespace nucleus {

// Forward declaration.
class ToyIterable;

class ToyReader : public Reader {
 private:
  // Underlying container.
  std::vector<StatusOr<string>> toys_;

 public:
  explicit ToyReader(const std::vector<StatusOr<string>>& toys) : toys_(toys) {}

  explicit ToyReader(const std::vector<string>& toys) {
    std::transform(toys.begin(), toys.end(), std::back_inserter(toys_),
                   [](const string& s) { return StatusOr<string>(s); });
  }

  std::shared_ptr<ToyIterable> IterateFrom(int startingPos = 0) {
    if (startingPos < 0) {
      LOG(ERROR) << "ToyIterable: bad starting pos";
      return nullptr;
    } else {
      return MakeIterable<ToyIterable>(this, startingPos);
    }
  }

  friend class ToyIterable;
};

class ToyIterable : public Iterable<string> {
 private:
  uint32 pos_;

 public:
  StatusOr<bool> Next(string* out) override {
    const ToyReader& reader = *static_cast<const ToyReader*>(reader_);
    TF_RETURN_IF_ERROR(CheckIsAlive());
    if (pos_ < reader.toys_.size()) {
      StatusOr<string> toy_or = reader.toys_[pos_];
      TF_RETURN_IF_ERROR(toy_or.status());
      *out = toy_or.ValueOrDie();
      ++pos_;
      return true;
    } else {
      return false;
    }
  }

  ToyIterable(const ToyReader* reader, int startingPos)
      : Iterable(reader), pos_(startingPos) {}

  ~ToyIterable() override {}
};

TEST(ReaderIterableTest, EmptyReaderRange) {
  ToyReader tr0(std::vector<string>{});
  int i = 0;
  for (const StatusOr<string*> toy_status : tr0.IterateFrom(0)) {
    ASSERT_THAT(toy_status, IsOK());
    toy_status.ValueOrDie();
    i++;
  }
  EXPECT_EQ(0, i);
}

TEST(ReaderIterableTest, SupportsBasicIteration) {
  std::vector<string> toys = {"ball", "doll", "house", "legos"};
  std::vector<string> from1 = {"doll", "house", "legos"};
  ToyReader tr(toys);
  auto it = tr.IterateFrom(1);
  auto b = begin(it);
  auto e = end(it);
  std::vector<string> gathered;
  for (auto cur = b; cur != e; ++cur) {
    gathered.push_back(*(*cur).ValueOrDie());
  }
  EXPECT_EQ(from1, gathered);
}

TEST(ReaderIterableTest, SupportsRangeFor) {
  std::vector<string> toys = {"ball", "doll", "house", "legos"};
  std::vector<string> from1 = {"doll", "house", "legos"};
  ToyReader tr(toys);
  std::vector<string> gathered;
  for (const StatusOr<string*> toy : tr.IterateFrom(1)) {
    ASSERT_THAT(toy, IsOK());
    gathered.push_back(*toy.ValueOrDie());
  }
  EXPECT_EQ(from1, gathered);
}

// Ensure that the Iterable Next() interface properly handles an error, for
// example as would be encountered upon parsing a malformed record in a file.
// This interface is used by our Python APIs.
TEST(ReaderIterableTest, IterationHandlesError) {
  ToyReader tr({StatusOr<string>("ball"),
                tf::errors::Unknown("Malformed record: argybarg"),
                StatusOr<string>("doll")});

  std::shared_ptr<ToyIterable> it = tr.IterateFrom(0);
  StatusOr<bool> not_eof_or;
  string line;

  not_eof_or = it->Next(&line);
  ASSERT_TRUE(not_eof_or.ok() && not_eof_or.ValueOrDie());
  ASSERT_EQ(line, "ball");

  not_eof_or = it->Next(&line);
  ASSERT_THAT(not_eof_or, IsNotOKWithMessage("Malformed record: argybarg"));

  // After initially encountering a failure, successive Next() calls will
  // continue to return the same error--we cannot advance further.
  not_eof_or = it->Next(&line);
  ASSERT_THAT(not_eof_or, IsNotOKWithMessage("Malformed record: argybarg"));
}

// Ensure that C++ iterator interface properly handles an error, for example as
// would be encountered upon parsing a malformed record in a file.
TEST(ReaderIterableTest, CppIterationHandlesError) {
  ToyReader tr({StatusOr<string>("ball"),
                tf::errors::Unknown("Malformed record: argybarg"),
                StatusOr<string>("doll")});
  auto it = tr.IterateFrom(0);
  auto it_cur = begin(it);
  auto it_end = end(it);

  ASSERT_FALSE(it_cur == it_end);
  ASSERT_THAT(*it_cur, IsOK());
  ASSERT_THAT((*it_cur).ValueOrDie(), Pointee(StrEq("ball")));

  ++it_cur;
  ASSERT_FALSE(it_cur == it_end);
  ASSERT_THAT(*it_cur, IsNotOKWithMessage("Malformed record: argybarg"));

  // We cannot advance any further once an error has been encountered.
  ++it_cur;
  ASSERT_TRUE(it_cur == it_end);
}

TEST(ReaderIterableTest, TestProtectionAgainstMultipleIteration) {
  ToyReader tr({"ball", "doll", "house", "legos"});

  // Scope for RAII auto-destruction of iterable.
  {
    auto it1 = tr.IterateFrom(0);
    auto it2 = tr.IterateFrom(0);

    // The first iterator is good; the second should be null because
    // we detected the attempt to get two concurrent iterators.
    EXPECT_NE(nullptr, it1);
    EXPECT_EQ(nullptr, it2);
  }
  // it1 has died, so we can get a new iterable successfully.
  auto it3 = tr.IterateFrom(0);
  EXPECT_NE(it3, nullptr);

  int i = 0;
  for (const StatusOr<string*> toy : it3) {
    ASSERT_THAT(toy, IsOK());
    (void)toy.ValueOrDie();
    i++;
  }
  EXPECT_EQ(i, 4);
}

TEST(ReaderIterableTest, TestExplicitRelease) {
  ToyReader tr({"ball", "doll", "house", "legos"});
  std::shared_ptr<ToyIterable> it1 = tr.IterateFrom(0);
  EXPECT_NE(it1, nullptr);
  std::shared_ptr<ToyIterable> it2 = tr.IterateFrom(0);
  EXPECT_EQ(it2, nullptr);
  ASSERT_THAT(it1->Release(), IsOK());
  std::shared_ptr<ToyIterable> it3 = tr.IterateFrom(0);
  EXPECT_NE(it3, nullptr);
}

TEST(ReaderIterableTest, TestReaderDiesBeforeIterable) {
  std::shared_ptr<ToyIterable> ti;
  {
    ToyReader tr({"ball", "doll", "house", "legos"});
    ti = tr.IterateFrom(0);
    string s;
    StatusOr<bool> status = ti->Next(&s);
    ASSERT_THAT(status, IsOK());
    EXPECT_TRUE(status.ValueOrDie());
  }
  // tr has been destructed; ti is about to be.  If ti doesn't know
  // the reader is dead, we are likely to crash here.  This can happen
  // in Python since destruction order is non-deterministic.
}

}  // namespace nucleus
