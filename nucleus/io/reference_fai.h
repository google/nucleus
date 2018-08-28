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

// Implementation of GenomeReference class using htslib FAI index format.
#ifndef THIRD_PARTY_NUCLEUS_IO_REFERENCE_FAI_H_
#define THIRD_PARTY_NUCLEUS_IO_REFERENCE_FAI_H_

#include <vector>

#include "absl/types/optional.h"
#include "htslib/faidx.h"
#include "nucleus/io/reference.h"
#include "nucleus/vendor/statusor.h"
#include "nucleus/platform/types.h"

namespace nucleus {

constexpr int REFERENCE_FAI_DEFAULT_CACHE_SIZE = 64 * 1024;


// A FASTA reader backed by a htslib FAI index.
//
// FASTA files store information about DNA/RNA/Amino Acid sequences:
//
// https://en.wikipedia.org/wiki/FASTA_format
//
// This reader is specialized for the FASTA variant used in NGS analyses, which
// has a FAI index created by samtools that allows efficient query() operations
// to obtain the subsequence of the FASTA on a specific contig between a start
// and end offsets:
//
// http://www.htslib.org/doc/faidx.html
// http://www.htslib.org/doc/samtools.html [faidx section]
//
// The FASTA file can be optionally block-gzipped compressed.
//
// This class provides methods to iterate through a the FASTA records and to
// also query() for the bases spanning a specific region on the genome.
//
// Uses the htslib C API for reading the FASTA and FAI. For details of
// the API, see:
//
// https://github.com/samtools/htslib/tree/develop/htslib
//
// The objects returned by iterate() or query() are strings containing the
// bases, all upper-cased.
class GenomeReferenceFai : public GenomeReference {
 public:
  // Creates a new GenomeReference backed by the FASTA file fasta_path.
  //
  // Returns this newly allocated GenomeReferenceFai object, passing ownership
  // to the caller via a unique_ptr.
  //
  // htslib currently assumes that the FAI file is named fasta_path + '.fai',
  // so that file must exist and be readable by htslib.
  //
  // We maintain a single entry cache of the bases from the last FASTA fetch, to
  // reduce the number of file reads, which can be quite costly for remote
  // filesystems.  64K is the default block size for htslib faidx fetches, so
  // there is no penalty to rounding up all small access sizes to 64K.  The
  // cache can be disabled using `cache_size=0`.
  static StatusOr<std::unique_ptr<GenomeReferenceFai>> FromFile(
      const string& fasta_path, const string& fai_path,
      int cache_size_bases = REFERENCE_FAI_DEFAULT_CACHE_SIZE);

  ~GenomeReferenceFai();

  // Disable copy and assignment operations
  GenomeReferenceFai(const GenomeReferenceFai& other) = delete;
  GenomeReferenceFai& operator=(const GenomeReferenceFai&) = delete;

  const std::vector<nucleus::genomics::v1::ContigInfo>& Contigs()
      const override {
    return contigs_;
  }

  StatusOr<string> GetBases(
      const nucleus::genomics::v1::Range& range) const override;

  StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>> Iterate()
      const override;

  // Close the underlying resource descriptors.
  tensorflow::Status Close() override;

 private:
  // Allow iteration to access the underlying reader.
  friend class GenomeReferenceFaiIterable;

  // Must use one of the static factory methods.
  GenomeReferenceFai(
      const string& fasta_path, faidx_t* faidx, int cache_size_bases);

  // Path to the FASTA file containing our genomic bases.
  const string fasta_path_;

  // Initialized htslib faidx_t index for our fasta. Effectively constant during
  // the life of this object, but htslib API doesn't allow us to mark this as
  // const.
  faidx_t* faidx_;

  // A list of ContigInfo, each of which contains the information about the
  // contigs used by this BAM file.
  const std::vector<nucleus::genomics::v1::ContigInfo> contigs_;

  // Size, in bases, of the read cache.
  const int cache_size_bases_;

  // Cache of the last "small" read from the FASTA (<= kFastaCacheSize).
  mutable string small_read_cache_;

  // The range that is held in the cache, or "empty" if there is no range cached
  // yet.  Range must be <= kFastaCacheSize in length.
  mutable absl::optional<nucleus::genomics::v1::Range> cached_range_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_REFERENCE_FAI_H_
