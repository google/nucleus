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
#include "nucleus/util/proto_clif_converter.h"
#include "clif/python/types.h"
#include "google/protobuf/message.h"
#include "net/proto2/python/public/proto_api.h"

namespace clif {

static const google::protobuf::python::PyProto_API* py_proto_api = nullptr;

const ::google::protobuf::Message* GetPyProtoMessagePointer(PyObject* py) {
  if (py_proto_api == nullptr) {
    py_proto_api = static_cast<const google::protobuf::python::PyProto_API*>(
        PyCapsule_Import(google::protobuf::python::PyProtoAPICapsuleName(), 0));
    if (py_proto_api == nullptr) {
      return nullptr;
    }
  }

  const ::google::protobuf::Message* cpb = py_proto_api->GetMessagePointer(py);
  return cpb;
}

}  // namespace clif
