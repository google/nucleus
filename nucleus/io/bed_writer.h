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

#ifndef THIRD_PARTY_NUCLEUS_IO_BED_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_BED_WRITER_H_

#include "nucleus/io/text_writer.h"
#include "nucleus/protos/bed.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/platform/types.h"

namespace nucleus {

using tensorflow::string;

// A BED writer, allowing us to write BED files.
//
// BED files flexibly store annotation information about a reference genome.
//
// https://genome.ucsc.edu/FAQ/FAQformat.html#format1
class BedWriter {
 public:
  // Creates a new BedWriter writing to the file at bed_path, which is
  // opened and created if needed. Returns either a unique_ptr to the
  // BedWriter or a Status indicating why an error occurred.
  static StatusOr<std::unique_ptr<BedWriter>> ToFile(
      const string& bed_path, const nucleus::genomics::v1::BedHeader& header,
      const nucleus::genomics::v1::BedWriterOptions& options);

  ~BedWriter();

  // Disable copy and assignment operations.
  BedWriter(const BedWriter& other) = delete;
  BedWriter& operator=(const BedWriter&) = delete;

  // Write a BedRecord to the BED file.
  // Returns Status::OK() if the write was successful; otherwise the status
  // provides information about what error occurred.
  tensorflow::Status Write(const nucleus::genomics::v1::BedRecord& record);

  // Close the underlying resource descriptors. Returns Status::OK() if the
  // close was successful; otherwise the status provides information about what
  // error occurred.
  tensorflow::Status Close();

  // Provide access to the header.
  const nucleus::genomics::v1::BedHeader& Header() const { return header_; }

  // This no-op function is needed only for Python context manager support.  Do
  // not use it!
  void PythonEnter() const {}

 private:
  // Private constructor; use ToFile to safely create a BedWriter.
  BedWriter(std::unique_ptr<TextWriter> text_writer,
            const nucleus::genomics::v1::BedHeader& header,
            const nucleus::genomics::v1::BedWriterOptions& options);

  // The header of the BED file.
  const nucleus::genomics::v1::BedHeader header_;

  // Our options that control the behavior of this class.
  const nucleus::genomics::v1::BedWriterOptions options_;

  // Underlying file writer.
  std::unique_ptr<TextWriter> text_writer_;

};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_BED_WRITER_H_
