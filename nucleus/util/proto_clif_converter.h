/*
 * Copyright 2018 Google LLC.
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
#ifndef THIRD_PARTY_NUCLEUS_UTIL_PROTO_CLIF_CONVERTER_H_
#define THIRD_PARTY_NUCLEUS_UTIL_PROTO_CLIF_CONVERTER_H_

#include "google/protobuf/message.h"
#include "python/google/protobuf/proto_api.h"
#include "clif/python/types.h"
#include "nucleus/util/proto_ptr.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

// Note: the comments below are instructions to CLIF.
// CLIF use `::nucleus::EmptyProtoPtr` as EmptyProtoPtr
// CLIF use `::nucleus::ConstProtoPtr` as ConstProtoPtr

const ::proto2::python::PyProto_API* GetPyProtoApi(PyObject* py);

// Convert from Python protocol buffer object py to a C++ pointer.
// Unlike the conversions that CLIF automatically generates for protocol
// buffers, this one does no copying if the Python protocol buffer uses
// the C++ memory layout.
template <typename T>
bool Clif_PyObjAs(PyObject* py, EmptyProtoPtr<T>* c) {
  CHECK(c != nullptr);

  auto* py_proto_api = GetPyProtoApi(py);
  if (py_proto_api == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Could not load PyProto API");
    return false;
  }

  ::proto2::Message* cpb = py_proto_api->GetMutableMessagePointer(py);
  if (cpb == nullptr) {
    // Clients might depend on our non-copying semantics, so we can't fall
    // back on CLIF here but instead must fail loudly.
    PyErr_SetString(PyExc_RuntimeError,
                    "Python protobuf did not contain a mutable C++ protobuf");
    return false;
  } else {
    c->p_ = dynamic_cast<T*>(cpb);
    if (c->p_ == nullptr) {
      // DO NOT DELETE THIS WARNING!  Without it, the above dynamic_cast
      // will fail when running from a Python 3 pip package.
      LOG(WARNING) << "Failed to cast type " << typeid(*cpb).name();
      PyErr_SetString(PyExc_RuntimeError, "Dynamic cast failed");
      return false;
    }
    return true;
  }
}

// Convert from Python protocol buffer object py to a C++ pointer.
// Unlike the conversions that CLIF automatically generates for protocol
// buffers, this one does no copying if the Python protocol buffer uses
// the C++ memory layout.
template <typename T>
bool Clif_PyObjAs(PyObject* py, ConstProtoPtr<T>* c) {
  CHECK(c != nullptr);

  auto* py_proto_api = GetPyProtoApi(py);
  if (py_proto_api == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Could not load PyProto API");
    return false;
  }

  const ::proto2::Message* cpb = py_proto_api->GetMessagePointer(py);
  if (cpb == nullptr) {
    // Clients might depend on our non-copying semantics, so we can't fall
    // back on CLIF here but instead must fail loudly.
    PyErr_SetString(PyExc_RuntimeError,
                    "Python protobuf did not contain a C++ protobuf");
    return false;
  } else {
    c->p_ = dynamic_cast<const T*>(cpb);
    if (c->p_ == nullptr) {
      // DO NOT DELETE THIS WARNING!  Without it, the above dynamic_cast
      // will fail when running from a Python 3 pip package.
      LOG(WARNING) << "Failed to cast type " << typeid(*cpb).name();
      PyErr_SetString(PyExc_RuntimeError, "Dynamic cast failed");
      return false;
    }
    return true;
  }
}

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_UTIL_PROTO_CLIF_CONVERTER_H_
