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

#ifndef THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_CLIF_CONVERTERS_H_
#define THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_CLIF_CONVERTERS_H_

#include "clif/python/postconv.h"
#include "clif/python/types.h"
#include "nucleus/vendor/statusor.h"

namespace clif {

// Note: comment below is an instruction to CLIF.
// CLIF use `nucleus::StatusOr` as StatusOr
// CLIF use `::tensorflow::Status` as Status

void ErrorFromStatus(const tensorflow::Status& status);

PyObject* Clif_PyObjFrom(const tensorflow::Status& c, py::PostConv);

template <typename T>
PyObject* Clif_PyObjFrom(const nucleus::StatusOr<T>& c, py::PostConv pc) {
  if (!c.ok()) {
    ErrorFromStatus(c.status());
    return nullptr;
  }
  return Clif_PyObjFrom(c.ValueOrDie(), pc.Get(0));
}

template <typename T>
PyObject* Clif_PyObjFrom(nucleus::StatusOr<T>&& c,
                         py::PostConv pc) {  // NOLINT:c++11
  if (!c.ok()) {
    ErrorFromStatus(c.status());
    return nullptr;
  }
  return Clif_PyObjFrom(c.ConsumeValueOrDie(), pc.Get(0));
}

}  // namespace clif

#endif  // THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_CLIF_CONVERTERS_H_
