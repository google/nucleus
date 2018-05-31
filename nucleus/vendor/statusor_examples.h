/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Examples of StatusOr usage in C++ for CLIF bindings and tests.
#ifndef THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_EXAMPLES_H_
#define THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_EXAMPLES_H_

#include "nucleus/vendor/statusor.h"
#include "nucleus/platform/types.h"

namespace nucleus {

using tensorflow::Status;

static StatusOr<int> MakeIntOK() {
  return 42;
}

static StatusOr<int> MakeIntFail() {
  return Status(tensorflow::error::INVALID_ARGUMENT, "MakeIntFail");
}

static StatusOr<string> MakeStrOK() {
  return string("hello");
}

static StatusOr<string> MakeStrOKStrippedType() {
  return string("hello");
}

static StatusOr<string> MakeStrFail() {
  return Status(tensorflow::error::INVALID_ARGUMENT, "MakeStrFail");
}

static StatusOr<std::unique_ptr<int>> MakeIntUniquePtrOK() {
  return std::unique_ptr<int>(new int(421));
}

static StatusOr<std::unique_ptr<int>> MakeIntUniquePtrFail() {
  return Status(tensorflow::error::INVALID_ARGUMENT, "MakeIntUniquePtrFail");
}

static StatusOr<std::unique_ptr<std::vector<int>>> MakeIntVectorOK() {
  return std::unique_ptr<std::vector<int>>(new std::vector<int>({1, 2, 42}));
}

static StatusOr<std::unique_ptr<std::vector<int>>> MakeIntVectorFail() {
  return Status(tensorflow::error::INVALID_ARGUMENT, "MakeIntVectorFail");
}

static Status FuncReturningStatusOK() {
  return Status::OK();
}

static Status FuncReturningStatusFail() {
  return Status(tensorflow::error::INVALID_ARGUMENT, "FuncReturningStatusFail");
}

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_EXAMPLES_H_
