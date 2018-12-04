/*
 * Copyright 2018 Google LLC.
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

// Note: comment below is an instruction to CLIF.
// CLIF use `::nucleus::StatusOr` as StatusOr
// CLIF use `::tensorflow::Status` as Status

namespace tensorflow {

// We do not own namespace tensorflow, but at this point, nobody else is going
// to provide the necessary conversion function for ::tensorflow::Status. This
// is less than ideal, but for now we provide the function here.

PyObject* Clif_PyObjFrom(const Status& c, const ::clif::py::PostConv&);

}  // namespace tensorflow

namespace nucleus {
namespace internal {

void ErrorFromStatus(const ::tensorflow::Status& status);

}  // namespace internal

template <typename T>
PyObject* Clif_PyObjFrom(const StatusOr<T>& c,
                         const ::clif::py::PostConv& pc) {
  if (!c.ok()) {
    internal::ErrorFromStatus(c.status());
    return nullptr;
  } else {
    using ::clif::Clif_PyObjFrom;
    return Clif_PyObjFrom(c.ValueOrDie(), pc.Get(0));
  }
}

template <typename T>
PyObject* Clif_PyObjFrom(StatusOr<T>&& c, const ::clif::py::PostConv& pc) {
  if (!c.ok()) {
    internal::ErrorFromStatus(c.status());
    return nullptr;
  } else {
    using ::clif::Clif_PyObjFrom;
    return Clif_PyObjFrom(c.ConsumeValueOrDie(), pc.Get(0));
  }
}

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_VENDOR_STATUSOR_CLIF_CONVERTERS_H_
