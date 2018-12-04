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
 *
 */

#ifndef THIRD_PARTY_NUCLEUS_IO_UNINDEXED_FASTA_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_UNINDEXED_FASTA_READER_H_

#include <memory>
#include <string>
#include <vector>

#include "absl/types/optional.h"
#include "htslib/faidx.h"
#include "nucleus/io/reference.h"
#include "nucleus/io/text_reader.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

// A FASTA reader that is not backed by a htslib FAI index.
//
// FASTA files store information about DNA/RNA/Amino Acid sequences:
//
// https://en.wikipedia.org/wiki/FASTA_format
//
// This reader is for FASTA files that contain many small records and are
// explicitly not indexed. The FASTA files can be optionally block-gzipped
// compressed.
//
// This class provides methods to iterate through a the FASTA records but
// doesn't support query() for the bases spanning a specific region on the
// genome.
//
// The (name, bases) tuple returned by iterate() are strings containing the
// bases in uppercase.
class UnindexedFastaReader : public GenomeReference {
 public:
  // Creates a new GenomeReference backed by the FASTA file fasta_path.
  //
  // Returns this newly allocated UnindexedFastaReader object, passing ownership
  // to the caller via a unique_ptr.
  static StatusOr<std::unique_ptr<UnindexedFastaReader>> FromFile(
      const string& fasta_path);

  ~UnindexedFastaReader();

  // Disable copy and assignment operations
  UnindexedFastaReader(const UnindexedFastaReader& other) = delete;
  UnindexedFastaReader& operator=(const UnindexedFastaReader&) = delete;

  const std::vector<nucleus::genomics::v1::ContigInfo>& Contigs()
      const override;

  StatusOr<string> GetBases(
      const nucleus::genomics::v1::Range& range) const override;

  StatusOr<std::shared_ptr<GenomeReferenceRecordIterable>> Iterate()
      const override;

  // Close the underlying resource descriptors.
  tensorflow::Status Close() override;

 private:
  // Allow iteration to access the underlying reader.
  friend class UnindexedFastaReaderIterable;

  // Must use one of the static factory methods.
  UnindexedFastaReader(std::unique_ptr<TextReader> text_reader);

  const std::vector<nucleus::genomics::v1::ContigInfo> contigs_;

  // Underlying file reader.
  std::unique_ptr<TextReader> text_reader_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_UNINDEXED_FASTA_READER_H_
