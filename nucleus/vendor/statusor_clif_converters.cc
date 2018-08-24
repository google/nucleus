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

#include "nucleus/vendor/statusor_clif_converters.h"

#include <string>

namespace tensorflow {

PyObject* Clif_PyObjFrom(const Status& c, const clif::py::PostConv&) {
  if (!c.ok()) {
    ::nucleus::internal::ErrorFromStatus(c);
    return nullptr;
  } else {
    Py_RETURN_NONE;
  }
}

}  // namespace tensorflow

namespace nucleus {
namespace internal {

void ErrorFromStatus(const tensorflow::Status& status) {
  PyErr_SetString(PyExc_ValueError, status.ToString().c_str());
}

}  // namespace internal
}  // namespace nucleus
