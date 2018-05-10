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
#ifndef THIRD_PARTY_NUCLEUS_UTIL_PROTO_CLIF_CONVERTER_H_
#define THIRD_PARTY_NUCLEUS_UTIL_PROTO_CLIF_CONVERTER_H_

#include "clif/python/types.h"
#include "google/protobuf/message.h"
#include "nucleus/vendor/python_protobuf.h"
#include "nucleus/util/proto_ptr.h"
#include "tensorflow/core/platform/logging.h"

namespace clif {

// Note: comment below is an instruction to CLIF.
// CLIF use `nucleus::ProtoPtr` as ProtoPtr

// Convert from Python protocol buffer object py to a C++ pointer.
// Unlike the conversions that CLIF automatically generates for protocol
// buffers, this one does no copying if the Python protocol buffer uses
// the C++ memory layout.
template <typename T>
bool Clif_PyObjAs(PyObject* py, nucleus::ProtoPtr<T>* c) {
  CHECK(c != nullptr);

  ::proto2::Message* cpb = google::protobuf::MutableCProtoInsidePyProto(py);
  if (cpb == nullptr) {
    // Clients might depend on our non-copying semantics, so we can't fall
    // back on CLIF here but instead must fail loudly.
    return false;
  }

  c->p_ = dynamic_cast<T*>(cpb);

  return true;
}

}  // namespace clif
#endif  // THIRD_PARTY_NUCLEUS_UTIL_PROTO_CLIF_CONVERTER_H_
