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

#include "nucleus/io/bedgraph_writer.h"

#include <utility>

#include "absl/memory/memory.h"
#include "absl/strings/substitute.h"
#include "nucleus/platform/types.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

namespace tf = tensorflow;

StatusOr<std::unique_ptr<BedGraphWriter>> BedGraphWriter::ToFile(
    const string& bedgraph_path) {
  StatusOr<std::unique_ptr<TextWriter>> text_writer =
      TextWriter::ToFile(bedgraph_path);
  TF_RETURN_IF_ERROR(text_writer.status());
  return absl::WrapUnique(new BedGraphWriter(text_writer.ConsumeValueOrDie()));
}

BedGraphWriter::~BedGraphWriter() {
  if (!text_writer_) {
    return;
  }
  tf::Status status = Close();
  if (!status.ok()) {
    LOG(WARNING) << "Closing BedGraphReader encountered an error";
  }
}

tf::Status BedGraphWriter::Close() {
  if (!text_writer_) {
    return tf::errors::FailedPrecondition(
        "Cannot close an already closed BedGraphWriter");
  }
  tf::Status close_status = text_writer_->Close();
  text_writer_ = nullptr;
  return close_status;
}

tf::Status BedGraphWriter::Write(
    const nucleus::genomics::v1::BedGraphRecord& record) {
  if (!text_writer_) {
    return tf::errors::FailedPrecondition(
        "Cannot write to closed bedgraph stream.");
  }
  TF_RETURN_IF_ERROR(text_writer_->Write(
      absl::Substitute("$0\t$1\t$2\t$3\n", record.reference_name(),
                       record.start(), record.end(), record.data_value())));
  return tf::Status::OK();
}

BedGraphWriter::BedGraphWriter(std::unique_ptr<TextWriter> text_writer)
    : text_writer_(std::move(text_writer)) {}

}  // namespace nucleus
