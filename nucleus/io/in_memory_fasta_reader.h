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
#ifndef THIRD_PARTY_NUCLEUS_IO_IN_MEMORY_FASTA_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_IN_MEMORY_FASTA_READER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nucleus/io/reference.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/vendor/statusor.h"

namespace nucleus {

// An FASTA reader backed by in-memory ReferenceSequence protos.
//
// FASTA files store information about DNA/RNA/Amino Acid sequences:
//
// https://en.wikipedia.org/wiki/FASTA_format
//
//
// An InMemoryRefReader provides the same API as GenomeReferenceFAI but doesn't
// fetch its data from an on-disk FASTA file but rather fetches the bases from
// an in-memory cache containing ReferenceSequence protos.
//
// In particular the GetBases(Range(chrom, start, end)) operation fetches bases
// from the tuple where chrom == chromosome, and then from the bases where the
// first base of bases starts at start. If start > 0, then the bases string is
// assumed to contain bases starting from that position in the region. For
// example, the record ('1', 10, 'ACGT') implies that
// GetBases(ranges.make_range('1', 11, 12)) will return the base 'C', as the 'A'
// base is at position 10. This makes it straightforward to cache a small region
// of a full chromosome without having to store the entire chromosome sequence
// in memory (potentially big!).
class InMemoryFastaReader : public GenomeReference {
 public:
  // Creates a new InMemoryFastaReader backed by ReferenceSequence protos.
  static StatusOr<std::unique_ptr<InMemoryFastaReader>> Create(
      const std::vector<nucleus::genomics::v1::ContigInfo>& contigs,
      const std::vector<nucleus::genomics::v1::ReferenceSequence>& seqs);

  // Disable copy and assignment operations
  InMemoryFastaReader(const InMemoryFastaReader& other) = delete;
  InMemoryFastaReader& operator=(const InMemoryFastaReader&) = delete;

  const std::vector<nucleus::genomics::v1::ContigInfo>& Contigs()
      const override {
    return contigs_;
  }

  const std::unordered_map<string, nucleus::genomics::v1::ReferenceSequence>&
  ReferenceSequences() const {
    return seqs_;
  }

  StatusOr<string> GetBases(
      const nucleus::genomics::v1::Range& range) const override;

  StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>> Iterate()
      const override;

 private:
  // Allow iteration to access the underlying reader.
  friend class FastaFullFileIterable;

  // Must use one of the static factory methods.
  InMemoryFastaReader(
      const std::vector<nucleus::genomics::v1::ContigInfo>& contigs,
      std::unordered_map<string, nucleus::genomics::v1::ReferenceSequence> seqs)
      : contigs_(contigs), seqs_(seqs) {}

  const std::vector<nucleus::genomics::v1::ContigInfo> contigs_;
  const std::unordered_map<string, nucleus::genomics::v1::ReferenceSequence>
      seqs_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_IN_MEMORY_FASTA_READER_H_
