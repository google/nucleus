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

#include "nucleus/io/fasta_reader.h"

#include <algorithm>

#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/core/errors.h"

namespace nucleus {

using nucleus::genomics::v1::Range;
using nucleus::genomics::v1::ReferenceSequence;

// Iterable class for traversing all Fasta records in the file.
class FastaFullFileIterable : public GenomeReferenceRecordIterable {
 public:
  // Advance to the next record.
  StatusOr<bool> Next(GenomeReferenceRecord* out) override;

  // Constructor is invoked via InMemoryGenomeReference::Iterate.
  FastaFullFileIterable(const InMemoryGenomeReference* reader);
  ~FastaFullFileIterable() override;

 private:
  int pos_ = 0;
};

// Initializes an InMemoryGenomeReference from contigs and seqs.
//
// contigs is a vector describing the "contigs" of this GenomeReference. These
// should include only the contigs present in seqs. A ContigInfo object for a
// contig `chrom` should describe the entire chromosome `chrom` even if the
// corresponding ReferenceSequence only contains a subset of the bases.
//
// seqs is a vector where each element describes a region of the genome we are
// caching in memory and will use to provide bases in the query() operation.
//
// Note that only a single ReferenceSequence for each contig is currently
// supported.
//
// There should be exactly one ContigInfo for each reference_name referred to
// across all ReferenceSequences, and no extra ContigInfos.
StatusOr<std::unique_ptr<InMemoryGenomeReference>>
InMemoryGenomeReference::Create(
      const std::vector<nucleus::genomics::v1::ContigInfo>& contigs,
      const std::vector<nucleus::genomics::v1::ReferenceSequence>& seqs) {
  std::unordered_map<string, nucleus::genomics::v1::ReferenceSequence> seqs_map;

  for (const auto& seq : seqs) {
    if (seq.region().reference_name().empty() || seq.region().start() < 0 ||
        seq.region().start() > seq.region().end()) {
      return tensorflow::errors::InvalidArgument(
          "Malformed region ", seq.region().ShortDebugString());
    }

    const size_t region_len = seq.region().end() - seq.region().start();
    if (region_len != seq.bases().length()) {
      return tensorflow::errors::InvalidArgument(
          "Region size = ", region_len, " not equal to bases.length() ",
          seq.bases().length());
    }

    auto insert_result = seqs_map.emplace(seq.region().reference_name(), seq);
    if (!insert_result.second) {
      return tensorflow::errors::InvalidArgument(
          "Each ReferenceSequence must be on a different chromosome but "
          "multiple ones were found on ", seq.region().reference_name());
    }
  }

  return std::unique_ptr<InMemoryGenomeReference>(
      new InMemoryGenomeReference(contigs, seqs_map));
}

StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>>
InMemoryGenomeReference::Iterate() const {
  return StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>>(
      MakeIterable<FastaFullFileIterable>(this));
}

StatusOr<string> InMemoryGenomeReference::GetBases(const Range& range) const {
  if (!IsValidInterval(range))
    return tensorflow::errors::InvalidArgument("Invalid interval: ",
                                               range.ShortDebugString());

  const ReferenceSequence& seq = seqs_.at(range.reference_name());

  if (range.start() < seq.region().start() ||
      range.end() > seq.region().end()) {
    return tensorflow::errors::InvalidArgument(
        "Cannot query range=", range.ShortDebugString(),
        " as this InMemoryRefReader only has bases in the interval=",
        seq.region().ShortDebugString());
  }
  const int64 pos = range.start() - seq.region().start();
  const int64 len = range.end() - range.start();
  return seq.bases().substr(pos, len);
}

StatusOr<bool> FastaFullFileIterable::Next(GenomeReferenceRecord* out) {
  TF_RETURN_IF_ERROR(CheckIsAlive());
  const InMemoryGenomeReference* fasta_reader =
      static_cast<const InMemoryGenomeReference*>(reader_);
  if (pos_ >= fasta_reader->contigs_.size()) {
    return false;
  }
  const string& reference_name = fasta_reader->contigs_.at(pos_).name();
  auto seq_iter = fasta_reader->seqs_.find(reference_name);
  if (seq_iter == fasta_reader->seqs_.end()) {
    return false;
  }
  DCHECK_NE(nullptr, out) << "FASTA record cannot be null";
  out->first = reference_name;
  out->second = seq_iter->second.bases();
  pos_++;
  return true;
}

FastaFullFileIterable::~FastaFullFileIterable() {}

FastaFullFileIterable::FastaFullFileIterable(
    const InMemoryGenomeReference* reader)
    : Iterable(reader) {}

}  // namespace nucleus
