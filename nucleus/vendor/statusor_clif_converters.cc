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

namespace clif {

void ErrorFromStatus(const tensorflow::Status& status) {
  const string message = status.ToString();
  PyErr_SetString(PyExc_ValueError, message.c_str());
}

PyObject* Clif_PyObjFrom(const tensorflow::Status& c, py::PostConv unused) {
  if (!c.ok()) {
    ErrorFromStatus(c);
    return nullptr;
  }
  Py_RETURN_NONE;
}

}  // namespace clif
