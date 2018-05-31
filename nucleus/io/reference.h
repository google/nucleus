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

// Get basic information about a reference genome as well as make it cpu and
// memory efficient and scalable to get the reference bases for an interval on
// the genome.
//
// The GenomeReference provides the core functionality needed to use a reference
// genome for data processing and analyses tools:
//
//   -- Get information about the contigs (aka chromosomes) present the FASTA,
//      such as its name, description, and number of basepairs.
//   -- Efficiently lookup the sequence of bases in an interval in the reference
//      genome. For example, GetBases("chr1", 0, 10) gets the basepair sequence
//      from the first base to the ninth base on chr1. This function call has
//      cost roughly proportional to the size of the query interval, regardless
//      of its position in the original FASTA file.
//
// The code here makes some strong assumptions about what a client could want.
// It doesn't record the position of bases in the original FASTA, and it doesn't
// track line breaks, comments, and other features of the FASTA. It uppercases
// the basepair sequences, so complexity or other information encoded in the
// case of the bases is lost. It also ensures that all of the bases in the
// reference are either {A,C,G,T,N} by refusing to import reference sequences
// containing other characters. The code assumes that random accesses of
// reasonably small chunks of sequence is important at the expense of low-cost
// (but still reasonably efficient) iteration of all sequences in the FASTA.
#ifndef THIRD_PARTY_NUCLEUS_IO_REFERENCE_H_
#define THIRD_PARTY_NUCLEUS_IO_REFERENCE_H_

#include <vector>

#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/vendor/statusor.h"
#include "nucleus/platform/types.h"

namespace nucleus {

class GenomeReference {
 public:
  GenomeReference(const GenomeReference&) = delete;
  GenomeReference& operator=(const GenomeReference&) = delete;

  virtual ~GenomeReference() {}

  virtual const std::vector<nucleus::genomics::v1::ContigInfo>& Contigs()
      const = 0;

  // Returns true if reference has a contig named contig_name.
  virtual bool HasContig(const string& contig_name) const;

  // Gets a vector of the contig names in this reference, in the order that they
  // appeared in the original FASTA file. For example, would return:
  //
  //    {"chrM", "chr1", "chr2"}
  //
  // for a reference with three contigs named chrM, chr1, and chr2.
  virtual std::vector<string> ContigNames() const;

  // Gets the metadata about a contig in the fasta, such as its name,
  // description, length, etc. If contig_name isn't found in this reference,
  // returns a value whose status is not ok().
  virtual StatusOr<const nucleus::genomics::v1::ContigInfo*> Contig(
      const string& contig_name) const;

  // Gets the basepairs in the FASTA file from Range range.
  //
  // This follows the Range convention of getting bases from start
  // (inclusive) to end (exclusive), both 0-based. That is,
  // GetBases(Range("chr1", 2, 4)) gets a string starting with the *3rd* base on
  // chr1 and extending through the *4th* base (excluding 4 as an offset).
  // If chr isn't present in this reference, start is invalid or end is beyond
  // the length of chr, returns a value whose status is not ok().
  virtual StatusOr<string> GetBases(
      const nucleus::genomics::v1::Range& range) const = 0;

  // Returns true iff the Range chr:start-end is a valid interval on chr and chr
  // is a known contig in this reference.
  bool IsValidInterval(const nucleus::genomics::v1::Range& range) const;

  // Close the underlying resource descriptors.
  virtual tensorflow::Status Close() { return tensorflow::Status::OK(); }

  // This no-op function is needed only for Python context manager support.  Do
  // not use it!
  virtual tensorflow::Status PythonEnter() const {
    return tensorflow::Status::OK();
  }

 protected:
  // Default constructor for subclasses.
  GenomeReference() {}
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_REFERENCE_H_
