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

namespace nucleus {

// Reader class methods

Reader::~Reader() {
  // If there is an outstanding iterable, we need to tell it that
  // the reader is dead so it doesn't still try to use it.
  absl::MutexLock lock(&mutex_);
  if (live_iterable_ != nullptr) {
    live_iterable_->reader_ = nullptr;
  }
}


// IterableBase class methods

IterableBase::IterableBase(const Reader* reader)
    : reader_(reader)
{}

IterableBase::~IterableBase() {
  // We cannot return a Status from our destructor, so the best we can do
  // if we need to release resources and cannot is CHECK-fail.
  TF_CHECK_OK(Release());
}

tensorflow::Status IterableBase::Release() {
  if (IsAlive()) {
    absl::MutexLock lock(&reader_->mutex_);
    if (reader_->live_iterable_ == nullptr) {
      return tensorflow::errors::FailedPrecondition(
          "reader_->live_iterable_ is null");
    }
    reader_->live_iterable_ = nullptr;
    reader_ = nullptr;
  }
  return tensorflow::Status::OK();
}

bool IterableBase::IsAlive() const {
  return reader_ != nullptr;
}

tensorflow::Status IterableBase::CheckIsAlive() const {
  if (!IsAlive())
    return tensorflow::errors::FailedPrecondition("Reader is not alive");
  return tensorflow::Status::OK();
}

tensorflow::Status IterableBase::PythonEnter() {
  return tensorflow::Status::OK();
}

tensorflow::Status IterableBase::PythonExit() {
  return Release();
}

}  // namespace nucleus
