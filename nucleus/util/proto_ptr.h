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
#ifndef THIRD_PARTY_NUCLEUS_UTIL_PROTO_PTR_H_
#define THIRD_PARTY_NUCLEUS_UTIL_PROTO_PTR_H_

// CLIF normally will serialize/deserialize protocol
// buffers when passing them from C++ to/from Python.
// These wrappers disable that default handling.
namespace nucleus {

// Use this wrapper when the C++ code fills in an EMPTY
// protocol buffer.  DO NOT use this to pass a non-empty
// protocol buffer from Python to C++; it will fail at
// runtime.
template <class T>
class EmptyProtoPtr {
 public:
  EmptyProtoPtr(T* p) : p_(p) {}
  EmptyProtoPtr() : p_(nullptr) {}

  T* p_;
};

// Use this wrapper when the C++ code reads, but does
// not modify, the Python protocol buffer.
template <class T>
class ConstProtoPtr {
 public:
  ConstProtoPtr(T* p) : p_(p) {}
  ConstProtoPtr() : p_(nullptr) {}

  T* p_;
};

}  // namespace nucleus
#endif  // THIRD_PARTY_NUCLEUS_UTIL_PROTO_PTR_H_
