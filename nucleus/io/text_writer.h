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

#ifndef THIRD_PARTY_NUCLEUS_IO_TEXT_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_TEXT_WRITER_H_

#include <memory>

#include "htslib/hts.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/types.h"

namespace nucleus {

using tensorflow::string;

// TextWriter is a class allowing writing text to a (possibly compressed) file
// stream
class TextWriter {
 public:  // Types.
  enum CompressionPolicy {
    NO_COMPRESS = false,
    COMPRESS = true,
  };

 public :
  // Factory function allowing explicit choice of whether to use compression.
  static StatusOr<std::unique_ptr<TextWriter>>
  ToFile(const string& path, CompressionPolicy compression);

  // Factory function that uses compression if the filename ends in ".gz".
  static StatusOr<std::unique_ptr<TextWriter>> ToFile(const string& path);

  // Destructor; closes the stream if still open.
  ~TextWriter();

  // Write a string to the file stream.
  tensorflow::Status Write(const string& text);

  // Close the underlying file stream.
  tensorflow::Status Close();

 private:
  // Private constructor.
  TextWriter(htsFile* hts_file);

  // Underlying htslib file stream.
  htsFile* hts_file_;
};

}  // namespace nucleus


#endif  // THIRD_PARTY_NUCLEUS_IO_TEXT_WRITER_H_
