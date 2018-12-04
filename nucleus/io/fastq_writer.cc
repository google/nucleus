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
 */

// Implementation of fastq_writer.h
#include "nucleus/io/fastq_writer.h"

#include <utility>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/fastq.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

namespace tf = tensorflow;

// 256 KB write buffer.
constexpr int WRITER_BUFFER_SIZE = 256 * 1024;

// -----------------------------------------------------------------------------
//
// Writer for FASTQ formats containing NGS reads.
//
// -----------------------------------------------------------------------------

StatusOr<std::unique_ptr<FastqWriter>> FastqWriter::ToFile(
    const string& fastq_path,
    const nucleus::genomics::v1::FastqWriterOptions& options) {
  StatusOr<std::unique_ptr<TextWriter>> text_writer =
      TextWriter::ToFile(fastq_path);
  TF_RETURN_IF_ERROR(text_writer.status());
  return absl::WrapUnique(
      new FastqWriter(text_writer.ConsumeValueOrDie(), options));
}

FastqWriter::FastqWriter(
    std::unique_ptr<TextWriter> text_writer,
    const nucleus::genomics::v1::FastqWriterOptions& options)
    : options_(options), text_writer_(std::move(text_writer)) {
}

FastqWriter::~FastqWriter() {
  if (text_writer_) {
    TF_CHECK_OK(Close());
  }
}

tf::Status FastqWriter::Close() {
  if (!text_writer_)
    return tf::errors::FailedPrecondition(
        "Cannot close an already closed FastqWriter");
  // Close the file pointer we have been writing to.
  tf::Status close_status = text_writer_->Close();
  text_writer_ = nullptr;
  return close_status;
}

tf::Status FastqWriter::Write(
    const nucleus::genomics::v1::FastqRecord& record) {
  if (!text_writer_)
    return tf::errors::FailedPrecondition(
        "Cannot write to closed FASTQ stream.");
  string out = "@";
  absl::StrAppend(&out, record.id());
  if (!record.description().empty()) {
    absl::StrAppend(&out, " ", record.description());
  }
  absl::StrAppend(&out, "\n", record.sequence(), "\n+\n", record.quality(),
                  "\n");
  TF_RETURN_IF_ERROR(text_writer_->Write(out));

  return tf::Status::OK();
}

}  // namespace nucleus
