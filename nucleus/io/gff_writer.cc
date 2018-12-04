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

#include "nucleus/io/gff_writer.h"

#include <limits>
#include <map>
#include <utility>

#include "google/protobuf/map.h"
#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/substitute.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/gff.pb.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

namespace tf = tensorflow;

namespace nucleus {

using genomics::v1::GffHeader;
using genomics::v1::GffRecord;
using genomics::v1::GffWriterOptions;
using genomics::v1::Range;

// Constants
// TODO(b/77959091): share these with the reader.
constexpr char kGffCommentPrefix[] = "#";
constexpr char kGffMissingField[] = ".";
constexpr double kGffMissingDouble = -std::numeric_limits<double>::infinity();
constexpr int32 kGffMissingInt32 = -1;

namespace {

tf::Status WriteGffHeader(const GffHeader& header, TextWriter* text_writer) {
  TF_RETURN_IF_ERROR(text_writer->Write("##gff-version 3.2.1\n"));
  for (const Range& range : header.sequence_regions()) {
    TF_RETURN_IF_ERROR(text_writer->Write(
        // Range start converted from 0- to 1-based, end-inclusive.
        absl::Substitute("##sequence-region $0 $1 $2\n", range.reference_name(),
                         range.start() + 1, range.end())));
  }
  // TODO(dhalexander): write ontology headers.
  return tf::Status::OK();
}

tf::Status FormatGffAttributes(const GffRecord& record,
                               string* gff_attributes) {
  // Sort to ensure deterministic iteration order.
  std::map<string, string> sorted_attributes(record.attributes().begin(),
                                             record.attributes().end());
  *gff_attributes =
      absl::StrJoin(sorted_attributes, ";", absl::PairFormatter("="));
  return tf::Status::OK();
}

tf::Status FormatGffLine(const GffRecord& record, string* gff_line) {
  string tmp, attributes;
  absl::StrAppend(&tmp, record.range().reference_name(), "\t");
  absl::StrAppend(
      &tmp, (record.source().empty() ? kGffMissingField : record.source()),
      "\t");
  absl::StrAppend(
      &tmp, (record.type().empty() ? kGffMissingField : record.type()), "\t");
  // Convert range to 1-based coordinates for GFF text.
  int64 start1 = record.range().start() + 1;
  int64 end1 = record.range().end();
  absl::StrAppend(&tmp, start1, "\t", end1, "\t");
  // Score
  string score_str =
      (record.score() == kGffMissingDouble ? kGffMissingField
                                           : absl::StrCat(record.score()));
  absl::StrAppend(&tmp, score_str, "\t");
  // Strand
  string strand_code;
  switch (record.strand()) {
    case GffRecord::UNSPECIFIED_STRAND:
      strand_code = kGffMissingField;
      break;
    case GffRecord::FORWARD_STRAND:
      strand_code = "+";
      break;
    case GffRecord::REVERSE_STRAND:
      strand_code = "-";
      break;
    default:
      return tf::errors::InvalidArgument("Illegal GffRecord strand encoding");
  }
  absl::StrAppend(&tmp, strand_code, "\t");
  // Phase
  int phase = record.phase();
  if (phase >= 0 && phase <= 2) {
    absl::StrAppend(&tmp, phase, "\t");
  } else if (phase == kGffMissingInt32) {
    absl::StrAppend(&tmp, kGffMissingField, "\t");
  } else {
    return tf::errors::InvalidArgument("Illegal GffRecord phase encoding");
  }
  // Attributes
  TF_RETURN_IF_ERROR(FormatGffAttributes(record, &attributes));
  absl::StrAppend(&tmp, attributes);
  absl::StrAppend(&tmp, "\n");

  *gff_line = tmp;
  return tf::Status::OK();
}

}  // namespace

StatusOr<std::unique_ptr<GffWriter>> GffWriter::ToFile(
    const string& gff_path, const GffHeader& header,
    const GffWriterOptions& options) {
  StatusOr<std::unique_ptr<TextWriter>> text_writer_or =
      TextWriter::ToFile(gff_path);
  TF_RETURN_IF_ERROR(text_writer_or.status());

  std::unique_ptr<TextWriter> text_writer =
      std::move(text_writer_or.ValueOrDie());
  TF_RETURN_IF_ERROR(WriteGffHeader(header, text_writer.get()));

  return absl::WrapUnique(
      new GffWriter(std::move(text_writer), header, options));
}

tf::Status GffWriter::Write(const GffRecord& record) {
  if (!text_writer_)
    return tf::errors::FailedPrecondition("Cannot write to closed GFF stream.");
  string line;
  TF_RETURN_IF_ERROR(FormatGffLine(record, &line));
  return text_writer_->Write(line);
}

tf::Status GffWriter::Close() {
  if (!text_writer_)
    return tf::errors::FailedPrecondition(
        "Cannot close an already closed GffWriter");
  // Close the file pointer we have been writing to.
  tf::Status close_status = text_writer_->Close();
  text_writer_ = nullptr;
  return close_status;
}

GffWriter::GffWriter(std::unique_ptr<TextWriter> text_writer,
                     const GffHeader& header, const GffWriterOptions& options)
    : header_(header),
      options_(options),
      text_writer_(std::move(text_writer)) {}

}  // namespace nucleus
