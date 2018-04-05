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

#include "htslib/kstring.h"
#include "nucleus/io/hts_path.h"
#include "nucleus/protos/fastq.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"

namespace nucleus {

namespace tf = tensorflow;

// For validation of the FASTQ format.
constexpr char HEADER_SYMBOL = '@';
constexpr char SEQUENCE_AND_QUALITY_SEPARATOR_SYMBOL = '+';

// -----------------------------------------------------------------------------
//
// Reader for FASTQ formats containing NGS reads.
//
// -----------------------------------------------------------------------------

namespace {
tf::Status ConvertToPb(const kstring_t& header, const kstring_t& sequence,
                       const kstring_t& pad, const kstring_t& quality,
                       nucleus::genomics::v1::FastqRecord* record) {
  CHECK(record != nullptr) << "FASTQ record cannot be null";
  if (!header.l || header.s[0] != HEADER_SYMBOL || !pad.l ||
      pad.s[0] != SEQUENCE_AND_QUALITY_SEPARATOR_SYMBOL || !sequence.l ||
      sequence.l != quality.l) {
    return tf::errors::DataLoss("Invalid FASTQ record");
  }
  record->Clear();
  char* spaceix = strchr(header.s, ' ');
  if (spaceix == NULL) {
    record->set_id(header.s + 1);
  } else {
    // ID is the string up to the first space.
    header.s[spaceix - header.s] = 0;
    record->set_id(header.s + 1);
    // Description is the string after the first space.
    record->set_description(spaceix + 1);
  }
  record->set_sequence(sequence.s);
  record->set_quality(quality.s);
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
  htsFile* fp = hts_open_x(fastq_path.c_str(), "r");
  if (fp == nullptr) {
    return tf::errors::NotFound("Could not open ", fastq_path);
  }
  return std::unique_ptr<FastqReader>(new FastqReader(fp, options));
}

FastqReader::FastqReader(
    htsFile* fp, const nucleus::genomics::v1::FastqReaderOptions& options)
    : options_(options), fp_(fp) {}

FastqReader::~FastqReader() {
  if (fp_) {
    TF_CHECK_OK(Close());
  }
}

tf::Status FastqReader::Close() {
  if (fp_ == nullptr) {
    return tf::errors::FailedPrecondition("FastqReader already closed");
  }
  int retval = hts_close(fp_);
  fp_ = nullptr;
  if (retval < 0) {
    return tf::errors::Internal("hts_close() failed with return code ", retval);
  }
  return tf::Status::OK();
}

tf::Status FastqReader::Next(kstring_t* header, kstring_t* sequence,
                             kstring_t* pad, kstring_t* quality) const {
  // Read the four lines, returning early if we are at the end of the stream or
  // the record is truncated.
  int ret = hts_getline(fp_, '\n', header);
  if (ret == -1) {
    return tf::errors::OutOfRange("");
  } else if (ret < 0)
    return tf::errors::DataLoss("Failed to parse FASTQ record");
  ret = hts_getline(fp_, '\n', sequence);
  if (ret < 0) return tf::errors::DataLoss("Failed to parse FASTQ record");
  ret = hts_getline(fp_, '\n', pad);
  if (ret < 0) return tf::errors::DataLoss("Failed to parse FASTQ record");
  ret = hts_getline(fp_, '\n', quality);
  if (ret < 0) return tf::errors::DataLoss("Failed to parse FASTQ record");
  return tf::Status::OK();
}

StatusOr<std::shared_ptr<FastqIterable>> FastqReader::Iterate() const {
  if (fp_ == nullptr)
    return tf::errors::FailedPrecondition(
        "Cannot Iterate a closed FastqReader.");
  return StatusOr<std::shared_ptr<FastqIterable>>(
      MakeIterable<FastqFullFileIterable>(this));
}

// Iterable class definitions.
StatusOr<bool> FastqFullFileIterable::Next(
    nucleus::genomics::v1::FastqRecord* out) {
  TF_RETURN_IF_ERROR(CheckIsAlive());
  const FastqReader* fastq_reader = static_cast<const FastqReader*>(reader_);
  kstring_t header = {0, 0, NULL}, sequence = {0, 0, NULL}, pad = {0, 0, NULL},
            quality = {0, 0, NULL};
  tf::Status status = fastq_reader->Next(&header, &sequence, &pad, &quality);
  if (!status.ok()) {
    free(header.s);
    free(sequence.s);
    free(pad.s);
    free(quality.s);
    if (tf::errors::IsOutOfRange(status)) {
      return false;
    } else {
      return status;
    }
  }
  status = ConvertToPb(header, sequence, pad, quality, out);
  free(header.s);
  free(sequence.s);
  free(pad.s);
  free(quality.s);
  TF_RETURN_IF_ERROR(status);
  return true;
}

FastqFullFileIterable::~FastqFullFileIterable() {}

FastqFullFileIterable::FastqFullFileIterable(const FastqReader* reader)
    : Iterable(reader) {}

}  // namespace nucleus
