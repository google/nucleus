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

// Implementation of bedgraph_reader.h
#include "nucleus/io/bedgraph_reader.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/bedgraph.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

namespace tf = tensorflow;

constexpr char BED_COMMENT_PREFIX[] = "#";

// -----------------------------------------------------------------------------
//
// Reader for BedGraph format data.
//
// -----------------------------------------------------------------------------

namespace {

tf::Status ConvertToPb(const string& line,
                       nucleus::genomics::v1::BedGraphRecord* record) {
  DCHECK_NE(nullptr, record) << "BedGraph record cannot be null";
  record->Clear();

  std::vector<string> tokens = absl::StrSplit(line, '\t');
  if (tokens.size() != 4) {
    return tf::errors::Unknown("BedGraph record has invalid number of fields");
  }
  record->set_reference_name(tokens[0]);
  int64 start, end = 0;
  if (!absl::SimpleAtoi(tokens[1], &start) ||
      !absl::SimpleAtoi(tokens[2], &end)) {
    return tf::errors::Unknown(
        "Unable to parse start and end positions in BedGraph");
  }
  record->set_start(start);
  record->set_end(end);
  double value = 0;
  if (!absl::SimpleAtod(tokens[3].c_str(), &value)) {
    return tf::errors::Unknown("Unable to parse data value in BedGraph");
  }
  record->set_data_value(value);
  return tf::Status::OK();
}

}  // namespace

// Iterable class for traversing all BedGraph records in the file.
class BedGraphFullFileIterable : public BedGraphIterable {
 public:
  // Advance to the next record.
  StatusOr<bool> Next(nucleus::genomics::v1::BedGraphRecord* out) override;

  // Constructor is invoked via BedGraphReader::Iterate.
  BedGraphFullFileIterable(const BedGraphReader* reader);
  ~BedGraphFullFileIterable() override;
};

StatusOr<std::unique_ptr<BedGraphReader>> BedGraphReader::FromFile(
    const string& bedgraph_path) {
  StatusOr<std::unique_ptr<TextReader>> status_or =
      TextReader::FromFile(bedgraph_path);
  TF_RETURN_IF_ERROR(status_or.status());
  return std::unique_ptr<BedGraphReader>(
      new BedGraphReader(std::move(status_or.ValueOrDie())));
}

BedGraphReader::BedGraphReader(std::unique_ptr<TextReader> text_reader)
    : text_reader_(std::move(text_reader)) {}

BedGraphReader::~BedGraphReader() {
  if (!text_reader_) {
    return;
  }
  tf::Status status = Close();
  if (!status.ok()) {
    LOG(WARNING) << "Closing BedGraphReader encountered an error";
  }
}

tf::Status BedGraphReader::Close() {
  if (!text_reader_) {
    return tf::errors::FailedPrecondition("BedGraphReader already closed");
  }
  tf::Status status = text_reader_->Close();
  text_reader_ = nullptr;
  return status;
}

StatusOr<std::shared_ptr<BedGraphIterable>> BedGraphReader::Iterate() const {
  if (!text_reader_) {
    return tf::errors::FailedPrecondition(
        "Cannot iterate a closed BedGraphReader");
  }
  return StatusOr<std::shared_ptr<BedGraphIterable>>(
      MakeIterable<BedGraphFullFileIterable>(this));
}

// Iterable class definitions.
StatusOr<bool> BedGraphFullFileIterable::Next(
    nucleus::genomics::v1::BedGraphRecord* out) {
  TF_RETURN_IF_ERROR(CheckIsAlive());
  const BedGraphReader* bedgraph_reader =
      static_cast<const BedGraphReader*>(reader_);
  string line;
  do {
    StatusOr<string> line_or = bedgraph_reader->text_reader_->ReadLine();
    if (!line_or.ok()) {
      if (tf::errors::IsOutOfRange(line_or.status())) {
        return false;
      }
      return line_or.status();
    }
    line = line_or.ValueOrDie();
  } while (absl::StartsWith(line, BED_COMMENT_PREFIX));
  TF_RETURN_IF_ERROR(ConvertToPb(line, out));
  return true;
}

BedGraphFullFileIterable::~BedGraphFullFileIterable() {}

BedGraphFullFileIterable::BedGraphFullFileIterable(const BedGraphReader* reader)
    : Iterable(reader) {}

}  // namespace nucleus
