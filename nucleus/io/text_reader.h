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

#ifndef THIRD_PARTY_NUCLEUS_IO_TEXT_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_TEXT_READER_H_

#include <memory>
#include <string>

#include "absl/memory/memory.h"
#include "htslib/hts.h"
#include "nucleus/platform/types.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {


// The TextReader class allows reading text from a (possibly compressed) file.
class TextReader {
 public:
  // Factory method to construct a TextReader.
  // File compression is determined from file magic (contents), not filename.
  static StatusOr<std::unique_ptr<TextReader>> FromFile(const string& path);

  // Destructor; closes the file, if it's still open.
  ~TextReader();

  // Reads a single line from the file.
  // Returns:
  //  - the string line (excluding trailing newline) if read is successful;
  //  - a status of tf::errors::OutOfRange if at end-of-file;
  //  - otherwise, an appropriate error Status.
  StatusOr<string> ReadLine();

  // Explicitly closes the underlying file stream.
  tensorflow::Status Close();

 private:
  // Private constructor.
  TextReader(htsFile* hts_file);

  // Underlying htslib file stream.
  htsFile* hts_file_;
};


}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_TEXT_READER_H_
