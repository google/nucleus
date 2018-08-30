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

#include "nucleus/io/unindexed_fasta_reader.h"

#include <stddef.h>
#include <utility>

#include "absl/strings/ascii.h"
#include "absl/strings/string_view.h"
#include "nucleus/io/reader_base.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

namespace tf = tensorflow;

namespace nucleus {

using genomics::v1::Range;

namespace {

// Helper method to get the name in a header line. This function assumes the
// first character is '>'.
absl::string_view GetNameInHeaderLine(absl::string_view line) {
  DCHECK_LT(1, line.size()) << "name must contain more than >";
  size_t space_idx = line.find(' ');
  if (space_idx == string::npos) {
    // No space is found. The name is the entire string after >.
    space_idx = line.size();
  }
  return line.substr(1, space_idx - 1);
}

}  // namespace

// Iterable class for traversing all Fasta records in the file.
class UnindexedFastaReaderIterable : public GenomeReferenceRecordIterable {
 public:
  // Advance to the next record.
  StatusOr<bool> Next(GenomeReferenceRecord* out) override;

  // Constructor is invoked via UnindexedFastaReader::Iterate.
  UnindexedFastaReaderIterable(const UnindexedFastaReader* reader);
  ~UnindexedFastaReaderIterable() override;

 private:
  // If non-empty, contains the name/id in the header line of the next record.
  std::string next_name_;
};

StatusOr<std::unique_ptr<UnindexedFastaReader>> UnindexedFastaReader::FromFile(
    const string& fasta_path) {
  StatusOr<std::unique_ptr<TextReader>> textreader_or =
      TextReader::FromFile(fasta_path);
  TF_RETURN_IF_ERROR(textreader_or.status());
  return std::unique_ptr<UnindexedFastaReader>(
      new UnindexedFastaReader(std::move(textreader_or.ValueOrDie())));
}

UnindexedFastaReader::~UnindexedFastaReader() {}

const std::vector<nucleus::genomics::v1::ContigInfo>&
UnindexedFastaReader::Contigs() const {
  LOG(FATAL) << "Unimplemented function invoked : " << __func__;
  return contigs_;
}

StatusOr<string> UnindexedFastaReader::GetBases(const Range& range) const {
  LOG(FATAL) << "Unimplemented function invoked : " << __func__;
  return tf::errors::Unimplemented("");
}

StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>>
UnindexedFastaReader::Iterate() const {
  return StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>>(
      MakeIterable<UnindexedFastaReaderIterable>(this));
}

tf::Status UnindexedFastaReader::Close() {
  if (!text_reader_) {
    return tf::errors::FailedPrecondition(
        "UnindexedFastaReader already closed");
  }
  // Close the file pointer.
  tf::Status close_status = text_reader_->Close();
  text_reader_ = nullptr;
  return close_status;
}

UnindexedFastaReader::UnindexedFastaReader(
    std::unique_ptr<TextReader> text_reader)
    : text_reader_(std::move(text_reader)) {}

StatusOr<bool> UnindexedFastaReaderIterable::Next(GenomeReferenceRecord* out) {
  TF_RETURN_IF_ERROR(CheckIsAlive());
  DCHECK(out && out->first.empty() && out->second.empty())
      << "out must be default initialized";

  const UnindexedFastaReader* fasta_reader =
      static_cast<const UnindexedFastaReader*>(reader_);
  if (!fasta_reader->text_reader_) {
    return tf::errors::FailedPrecondition(
        "Cannot iterate a closed UnindexedFastaReader.");
  }
  if (!next_name_.empty()) {
    out->first = next_name_;
    next_name_.clear();
  }
  bool eof = false;
  while (true) {
    // Read one line.
    StatusOr<string> line = fasta_reader->text_reader_->ReadLine();
    if (!line.ok()) {
      if (tf::errors::IsOutOfRange(line.status())) {
        eof = true;
        break;
      }
      return tf::errors::DataLoss("Failed to parse FASTA");
    }
    std::string l = line.ValueOrDie();

    if (l.empty()) continue;
    // Check if the line is a header or a sequence.
    if (l.at(0) == '>') {
      absl::string_view parsed_name = GetNameInHeaderLine(l);
      if (out->first.empty()) {
        out->first = string(parsed_name);
        continue;
      }
      next_name_ = string(parsed_name);
      return true;
    }
    // Processing a sequence line. If name is absent by now, return an error.
    if (out->first.empty()) {
      return tf::errors::DataLoss("Name not found in FASTA");
    }
    out->second.append(
        absl::AsciiStrToUpper(absl::StripTrailingAsciiWhitespace(l)));
  }
  if (eof && out->first.empty()) {
    // No more records.
    return false;
  }
  return true;
}

UnindexedFastaReaderIterable::~UnindexedFastaReaderIterable() {}

UnindexedFastaReaderIterable::UnindexedFastaReaderIterable(
    const UnindexedFastaReader* reader)
    : Iterable(reader) {}

}  // namespace nucleus
