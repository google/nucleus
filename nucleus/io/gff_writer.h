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

#ifndef THIRD_PARTY_NUCLEUS_IO_GFF_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_GFF_WRITER_H_

#include <memory>
#include <string>

#include "nucleus/io/text_writer.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/gff.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

const nucleus::genomics::v1::GffWriterOptions kDefaultGffWriterOptions{};

// A GFF writer class, allowing us to write GFF (text) files.
// The GFF format is described here:
//   https://github.com/The-Sequence-Ontology/Specifications/blob/master/gff3.md
class GffWriter {
 public:
  // Creates a new GffWriter writing to the file at gff_path, which is
  // opened and created if needed. Returns either a unique_ptr to the
  // GffWriter or a Status indicating why an error occurred.  If
  // gff_path ends in ".gz", the resulting file will be GZIP compressed.
  static StatusOr<std::unique_ptr<GffWriter>> ToFile(
      const string& gff_path, const nucleus::genomics::v1::GffHeader& header,
      const nucleus::genomics::v1::GffWriterOptions& options =
          kDefaultGffWriterOptions);

  ~GffWriter() = default;

  // Disable copy and assignment operations.
  GffWriter(const GffWriter& other) = delete;
  GffWriter& operator=(const GffWriter&) = delete;

  // Writes a GffRecord to the GFF file.
  // Returns Status::OK() if the write was successful; otherwise the status
  // provides information about what error occurred.
  tensorflow::Status Write(const nucleus::genomics::v1::GffRecord& record);

  // Closes the underlying resource descriptors. Returns Status::OK() if the
  // close was successful; otherwise the status provides information about what
  // error occurred.
  tensorflow::Status Close();

  // Provides access to the header.
  const nucleus::genomics::v1::GffHeader& Header() const { return header_; }

  // This no-op function is needed only for Python context manager support.  Do
  // not use it!
  void PythonEnter() const {}

 private:
  // Private constructor; use ToFile to safely create a GffWriter.
  GffWriter(std::unique_ptr<TextWriter> text_writer,
            const nucleus::genomics::v1::GffHeader& header,
            const nucleus::genomics::v1::GffWriterOptions& options);

  // The header of the GFF file.
  const nucleus::genomics::v1::GffHeader header_;

  // Our options that control the behavior of this class.
  const nucleus::genomics::v1::GffWriterOptions options_;

  // Underlying file writer.
  std::unique_ptr<TextWriter> text_writer_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_GFF_WRITER_H_
