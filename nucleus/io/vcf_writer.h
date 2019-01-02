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

#ifndef THIRD_PARTY_NUCLEUS_IO_VCF_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_VCF_WRITER_H_

#include <memory>
#include <string>

#include "htslib/hts.h"
#include "htslib/sam.h"
#include "htslib/vcf.h"
#include "nucleus/io/vcf_conversion.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/variants.pb.h"
#include "nucleus/util/proto_ptr.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {


// A VCF writer, allowing us to write VCF files.
class VcfWriter {
 public:
  // Creates a new VcfWriter writing to the file at variants_path, which is
  // opened and created if needed. Returns either a unique_ptr to the VcfWriter
  // or a Status indicating why an error occurred.
  static StatusOr<std::unique_ptr<VcfWriter>> ToFile(
      const string& variants_path,
      const nucleus::genomics::v1::VcfHeader& header,
      const nucleus::genomics::v1::VcfWriterOptions& options);
  ~VcfWriter();

  // Disable copy or assignment
  VcfWriter(const VcfWriter& other) = delete;
  VcfWriter& operator=(const VcfWriter&) = delete;

  // Write a variant record to the VCF.
  // Note that variant calls must be provided in the same order as samples
  // listed in the options. Returns Status::OK() if the write was successful;
  // otherwise the status provides information about what error occurred.
  tensorflow::Status Write(
      const nucleus::genomics::v1::Variant& variant_message);
  tensorflow::Status WritePython(
      const ConstProtoPtr<const nucleus::genomics::v1::Variant>& wrapped) {
    return Write(*(wrapped.p_));
  }

  // Close the underlying resource descriptors. Returns Status::OK() if the
  // close was successful; otherwise the status provides information about what
  // error occurred.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.  Do
  // not use it!
  void PythonEnter() const {}

  // Access to the record converter.
  const VcfRecordConverter& RecordConverter() const {
    return record_converter_;
  }

 private:
  VcfWriter(const nucleus::genomics::v1::VcfHeader& header,
            const nucleus::genomics::v1::VcfWriterOptions& options,
            htsFile* fp);

  tensorflow::Status WriteHeader();

  // A pointer to the htslib file used to write the VCF data.
  htsFile* fp_;

  // The options controlling the behavior of this VcfWriter.
  const nucleus::genomics::v1::VcfWriterOptions options_;

  // The VcfHeader proto representation of the VCF header.
  const nucleus::genomics::v1::VcfHeader vcf_header_;

  // A pointer to the VCF header object.
  bcf_hdr_t* header_;

  // VCF record interconverter.
  VcfRecordConverter record_converter_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_VCF_WRITER_H_
