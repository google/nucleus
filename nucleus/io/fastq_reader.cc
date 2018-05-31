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

// Implementation of fastq_reader.h
#include "nucleus/io/fastq_reader.h"

#include "absl/strings/string_view.h"
#include "nucleus/protos/fastq.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"
#include "nucleus/platform/types.h"

namespace nucleus {

namespace tf = tensorflow;

using absl::string_view;
using nucleus::genomics::v1::FastqReaderOptions;
using nucleus::genomics::v1::FastqRecord;


// For validation of the FASTQ format.
constexpr char HEADER_SYMBOL = '@';
constexpr char SEQUENCE_AND_QUALITY_SEPARATOR_SYMBOL = '+';

// -----------------------------------------------------------------------------
//
// Reader for FASTQ formats containing NGS reads.
//
// -----------------------------------------------------------------------------


namespace {

// TODO(b/79165137): get rid of pessimizing string_view -> string conversions
// once our OSS dependencies are updated.
tf::Status ConvertToPb(const string_view header,
                       const string_view sequence,
                       const string_view pad,
                       const string_view quality,
                       nucleus::genomics::v1::FastqRecord* record) {
  CHECK(record != nullptr) << "FASTQ record cannot be null";
  if (header.empty() || header[0] != HEADER_SYMBOL ||
      pad.empty() || pad[0] != SEQUENCE_AND_QUALITY_SEPARATOR_SYMBOL ||
      sequence.empty() || sequence.length() != quality.length()) {
    return tf::errors::DataLoss("Invalid FASTQ record");
  }
  record->Clear();
  size_t spaceix = header.find(' ');
  if (spaceix == string::npos) {
    // No space found; ID is full string after delimiter.
    record->set_id(string(header.substr(1)));
  } else {
    // ID is the string from delimiter up to the first space.
    record->set_id(string(header.substr(1, spaceix - 1)));
    // Description is the string after the first space.
    record->set_description(string(header.substr(spaceix + 1)));
  }
  record->set_sequence(string(sequence));
  record->set_quality(string(quality));
  return tf::Status::OK();
}
}  // namespace

// Iterable class for traversing all FASTQ records in the file.
class FastqFullFileIterable : public FastqIterable {
 public:
  // Advance to the next record.
  StatusOr<bool> Next(nucleus::genomics::v1::FastqRecord* out) override;

  // Constructor is invoked via FastqReader::Iterate.
  FastqFullFileIterable(const FastqReader* reader);
  ~FastqFullFileIterable() override;
};

StatusOr<std::unique_ptr<FastqReader>> FastqReader::FromFile(
    const string& fastq_path,
    const nucleus::genomics::v1::FastqReaderOptions& options) {
  StatusOr<std::unique_ptr<TextReader>> textreader_or = TextReader::FromFile(fastq_path);
  TF_RETURN_IF_ERROR(textreader_or.status());
  return std::unique_ptr<FastqReader>(
      new FastqReader(std::move(textreader_or.ValueOrDie()), options));
}

FastqReader::FastqReader(std::unique_ptr<TextReader> text_reader,
                         const FastqReaderOptions& options)
    : options_(options), text_reader_(std::move(text_reader)) {}

FastqReader::~FastqReader() {
  if (text_reader_) {
    TF_CHECK_OK(Close());
  }
}

tf::Status FastqReader::Close() {
  if (!text_reader_) {
    return tf::errors::FailedPrecondition("FastqReader already closed");
  }
  // Close the file pointer.
  tf::Status close_status = text_reader_->Close();
  text_reader_ = nullptr;
  return close_status;
}

tf::Status FastqReader::Next(string* header, string* sequence,
                             string* pad, string* quality) const {
  // Read the four lines, returning early if we are at the end of the stream or
  // the record is truncated.
  StatusOr<string> header_or, sequence_or, pad_or, quality_or;

  header_or = text_reader_->ReadLine();
  if (!header_or.ok()) {
    if (tf::errors::IsOutOfRange(header_or.status())) {
      return header_or.status();
    } else { goto data_loss; }
  }
  sequence_or = text_reader_->ReadLine();
  if (!sequence_or.ok()) { goto data_loss; }

  pad_or = text_reader_->ReadLine();
  if (!pad_or.ok()) { goto data_loss; }

  quality_or = text_reader_->ReadLine();
  if (!quality_or.ok()) { goto data_loss; }

  *header = header_or.ValueOrDie();
  *sequence = sequence_or.ValueOrDie();
  *pad = pad_or.ValueOrDie();
  *quality = quality_or.ValueOrDie();
  return tf::Status::OK();

data_loss:
  return tf::errors::DataLoss("Failed to parse FASTQ record");
}

StatusOr<std::shared_ptr<FastqIterable>> FastqReader::Iterate() const {
  if (!text_reader_) {
    return tf::errors::FailedPrecondition(
        "Cannot Iterate a closed FastqReader.");
  }
  return StatusOr<std::shared_ptr<FastqIterable>>(
      MakeIterable<FastqFullFileIterable>(this));
}

// Iterable class definitions.
StatusOr<bool> FastqFullFileIterable::Next(FastqRecord* out) {
  TF_RETURN_IF_ERROR(CheckIsAlive());
  const FastqReader* fastq_reader = static_cast<const FastqReader*>(reader_);
  string header, sequence, pad, quality;
  tf::Status status = fastq_reader->Next(&header, &sequence, &pad, &quality);
  if (!status.ok()) {
    if (tf::errors::IsOutOfRange(status)) {
      return false;
    } else {
      return status;
    }
  }
  TF_RETURN_IF_ERROR(ConvertToPb(header, sequence, pad, quality, out));
  return true;
}

FastqFullFileIterable::~FastqFullFileIterable() {}

FastqFullFileIterable::FastqFullFileIterable(const FastqReader* reader)
    : Iterable(reader) {}

}  // namespace nucleus
