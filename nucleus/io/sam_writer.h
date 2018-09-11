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

#ifndef THIRD_PARTY_NUCLEUS_IO_SAM_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_SAM_WRITER_H_

#include <memory>
#include <string>

#include "htslib/hts.h"
#include "htslib/sam.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/reads.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

// A SAM/BAM/CRAM writer.
//
// SAM/BAM/CRAM files store information about a biological sequence and its
// corresponding quality scores.
//
// https://samtools.github.io/hts-specs/SAMv1.pdf
// https://samtools.github.io/hts-specs/CRAMv3.pdf
// This class converts nucleus.genomics.v1.SamHeader and
// nucleus.genomics.v1.Read to a file based on the file path passed in.
//
// This uses the htslib C API for writing NGS reads (BAM, SAM, SAM). For details
// of the API, see:
// https://github.com/samtools/htslib/tree/develop/htslib
//
// TODO(xunjieli): Maybe support writing out embed ref for CRAM format.
class SamWriter {
 public:
  // Creates a new SamWriter writing to the file at |sam_path|, which is
  // opened and created if needed. Returns either a unique_ptr to the
  // SamWriter or a Status indicating why an error occurred.
  static StatusOr<std::unique_ptr<SamWriter>> ToFile(
      const string& sam_path,
      const nucleus::genomics::v1::SamHeader& sam_header);

  // Creates a new SamWriter writing to the file at |sam_path|, which is
  // opened and created if needed. Additionally uses the reference FASTA file
  // at |ref_path| if |sam_path| has .cram extension. Returns either a
  // unique_ptr to the SamWriter or a Status indicating why an error occurred.
  static StatusOr<std::unique_ptr<SamWriter>> ToFile(
      const string& sam_path, const string& ref_path,
      const nucleus::genomics::v1::SamHeader& sam_header);

  ~SamWriter();

  // Disable copy and assignment operations.
  SamWriter(const SamWriter& other) = delete;
  SamWriter& operator=(const SamWriter&) = delete;

  // Write a Read to the  file.
  // Returns Status::OK() if the write was successful; otherwise the status
  // provides information about what error occurred.
  tensorflow::Status Write(const nucleus::genomics::v1::Read& read);

  // Close the underlying resource descriptors. Returns Status::OK() if the
  // close was successful; otherwise the status provides information about what
  // error occurred.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support. Do
  // not use it!
  void PythonEnter() const {}

 private:
  class NativeHeader;
  class NativeFile;
  class NativeBody;
  // Private constructor; use ToFile to safely create a SamWriter.
  SamWriter(std::unique_ptr<NativeFile> file,
            std::unique_ptr<NativeHeader> header);

  // A pointer to the htslib file used to access the SAM/BAM/CRAM data.
  std::unique_ptr<NativeFile> native_file_;

  // A htslib header data structure obtained by parsing the header of this file.
  std::unique_ptr<NativeHeader> native_header_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_SAM_WRITER_H_
