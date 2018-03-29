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
 */

#ifndef THIRD_PARTY_NUCLEUS_IO_FASTQ_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_FASTQ_WRITER_H_

#include "nucleus/protos/fastq.pb.h"
#include "nucleus/vendor/statusor.h"
#include "nucleus/vendor/zlib_outputbuffer.h"
#include "tensorflow/core/platform/file_system.h"
#include "tensorflow/core/platform/types.h"

namespace nucleus {

using tensorflow::string;

// A FASTQ writer, allowing us to write FASTQ files.
//
// FASTQ files store information about a biological sequence and its
// corresponding quality scores.
//
// https://en.wikipedia.org/wiki/FASTQ_format
class FastqWriter {
 public:
  // Creates a new FastqWriter writing to the file at fastq_path, which is
  // opened and created if needed. Returns either a unique_ptr to the
  // FastqWriter or a Status indicating why an error occurred.
  static StatusOr<std::unique_ptr<FastqWriter>> ToFile(
      const string& fastq_path,
      const nucleus::genomics::v1::FastqWriterOptions& options);

  ~FastqWriter();

  // Disable copy and assignment operations.
  FastqWriter(const FastqWriter& other) = delete;
  FastqWriter& operator=(const FastqWriter&) = delete;

  // Write a FastqRecord to the FASTQ file.
  // Returns Status::OK() if the write was successful; otherwise the status
  // provides information about what error occurred.
  tensorflow::Status Write(const nucleus::genomics::v1::FastqRecord& record);

  // Close the underlying resource descriptors. Returns Status::OK() if the
  // close was successful; otherwise the status provides information about what
  // error occurred.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.  Do
  // not use it!
  void PythonEnter() const {}

 private:
  // Private constructor; use ToFile to safely create a FastqWriter.
  FastqWriter(std::unique_ptr<tensorflow::WritableFile> fp,
              const nucleus::genomics::v1::FastqWriterOptions& options,
              const bool isCompressed);

  // Our options that control the behavior of this class.
  const nucleus::genomics::v1::FastqWriterOptions options_;

  // The file pointer for the given FASTQ path. The FastqWriter owns its file
  // pointer and is responsible for its deletion. Must outlive writer_.
  std::shared_ptr<tensorflow::WritableFile> raw_file_;

  // The file pointer used to write. For uncompressed output, this is the same
  // as raw_file_, but for compressed output it is distinct.
  std::shared_ptr<tensorflow::WritableFile> writer_;

  // Whether the output is written with compression.
  const bool isCompressed_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_FASTQ_WRITER_H_
