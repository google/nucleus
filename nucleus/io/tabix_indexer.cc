/*
 * Copyright 2019 Google LLC.
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

#include "nucleus/io/tabix_indexer.h"

#include "nucleus/io/hts_path.h"
#include "nucleus/platform/types.h"
#include "tensorflow/core/lib/core/errors.h"

namespace nucleus {

namespace tf = tensorflow;

tf::Status TbxIndexBuild(const string& path) {
  int val = tbx_index_build_x(path, 0, &tbx_conf_vcf);
  if (val < 0) {
    LOG(WARNING) << "Return code: " << val << "\nFile path: " << path;
    return tf::errors::Internal("Failure to write tabix index.");
  }
  return tf::Status::OK();
}

tf::Status CSIIndexBuild(string path, int min_shift) {
  // Create a index file in CSI format by setting min_shift as a non-zero value.
  int val = tbx_index_build_x(path, min_shift, &tbx_conf_vcf);
  if (val < 0) {
    LOG(WARNING) << "Return code: " << val << "\nFile path: " << path;
    return tf::errors::Internal("Failure to write CSI index.");
  }
  return tf::Status::OK();
}

}  // namespace nucleus
