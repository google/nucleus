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

#ifndef THIRD_PARTY_NUCLEUS_IO_SAM_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_SAM_READER_H_

#include <memory>
#include <string>

#include "htslib/hts.h"
#include "htslib/sam.h"
#include "nucleus/io/reader_base.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/reads.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/util/samplers.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {


// Alias for the abstract base class for SAM record iterables.
using SamIterable = Iterable<nucleus::genomics::v1::Read>;

// A SAM/BAM/CRAM reader.
//
// SAM/BAM/CRAM files store information about next-generation DNA sequencing
// info:
//
// https://samtools.github.io/hts-specs/SAMv1.pdf
// https://samtools.github.io/hts-specs/CRAMv3.pdf
//
// These files are block-gzipped series of records. When aligned they are
// frequently sorted and indexed:
//
// http://www.htslib.org/doc/samtools.html
//
// This class provides methods to iterate through a BAM file or, if indexed,
// to also query() for only read overlapping a specific region on the genome.
//
// Uses the htslib C API for reading NGS reads (BAM, SAM, CRAM etc). For details
// of the API, see:
//
// https://github.com/samtools/htslib/tree/develop/htslib
//
// The objects returned by iterate() or query() are nucleus.genomics.v1.Read
// objects parsed from the SAM/BAM/CRAM records in the file. Currently all
// fields except the extended key/value maps in each BAM fields are parsed.
//
class SamReader : public Reader {
 public:
  // Creates a new SamReader reading from the SAM/BAM/CRAM file reads_path.
  //
  // reads_path must point to an existing SAM/BAM/CRAM formatted file (text SAM,
  // compressed or uncompressed BAM file, CRAM files in all sorts of flavors).
  //
  // ref_path can be "", in which case the argument is ignored, or must point
  // to an existing FASTA file. If not "" and the reads_path points to a CRAM
  // file, the CRAM_OPT_REFERNECE field will be set to this path so that the
  // CRAM decoder uses ref_path to decode the reference-compressed read
  // sequences in the CRAM file. Because many low-level IO routines (e.g. stat)
  // are currently directly used in the CRAM implementation in htslib, ref_path
  // must be on a local (e.g., POSIX accessible) mount point. File system access
  // provided by htslib plugins (e.g., S3) won't work.
  //
  // If the filetype is BAM/CRAM, this constructor will attempt to load a BAI or
  // CRAI index from file reads_path + '.bai' or reads_path (without the .bam
  // extension) + '.bai'; if the index is not found, attempts to Query will
  // fail.
  //
  // Returns a StatusOr that is OK if the SamReader could be successfully
  // created or an error code indicating the error that occurred.
  static StatusOr<std::unique_ptr<SamReader>> FromFile(
      const string& reads_path,
      const string& ref_path,
      const nucleus::genomics::v1::SamReaderOptions& options);

  static StatusOr<std::unique_ptr<SamReader>> FromFile(
      const string& reads_path,
      const nucleus::genomics::v1::SamReaderOptions& options) {
    return FromFile(reads_path, "", options);
  }

  ~SamReader();

  // Disable assignment/copy operations
  SamReader(const SamReader& other) = delete;
  SamReader& operator=(const SamReader&) = delete;

  // Gets all of the reads in this file in order.
  //
  // This function allows one to iterate through all of the reads in this
  // SAM/BAM/CRAM file in order.
  //
  // The specific parsing, filtering, etc behavior is determined by the options
  // provided during construction. Returns an OK status if the iterable can be
  // constructed, or not OK otherwise.
  StatusOr<std::shared_ptr<SamIterable>> Iterate() const;

  // Gets all of the reads that overlap any bases in range.
  //
  // This function allows one to iterate through all of the reads in this
  // SAM/BAM/CRAM file in order that overlap a specific interval on the genome.
  // The query operation is efficient in that the cost is O(n) for n elements
  // that overlap range, and not O(N) for N elements in the entire file.
  //
  // The specific parsing, filtering, etc behavior is determined by the options
  // provided during construction.
  //
  // If no index was loaded by the constructor a non-OK status value will be
  // returned.
  //
  // If range isn't a valid interval in this BAM file a non-OK status value will
  // be returned.
  StatusOr<std::shared_ptr<SamIterable>> Query(
      const nucleus::genomics::v1::Range& region) const;

  // Returns True if this SamReader loaded an index file.
  bool HasIndex() const { return idx_ != nullptr; }

  // Close the underlying resource descriptors. Returns a Status to indicate if
  // everything went OK with the close.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.  Do
  // not use it! Returns a Status indicating whether the enter was successful.
  tensorflow::Status PythonEnter() const { return tensorflow::Status::OK(); }

  bool KeepRead(const nucleus::genomics::v1::Read& read) const;

  const nucleus::genomics::v1::SamReaderOptions& options() const {
    return options_;
  }

  // Returns a SamHeader message representing the structured header information.
  const nucleus::genomics::v1::SamHeader& Header() const { return sam_header_; }

 private:
  // Private constructor; use FromFile to safely create a SamReader from a
  // file.
  SamReader(const string& reads_path,
            const nucleus::genomics::v1::SamReaderOptions& options, htsFile* fp,
            bam_hdr_t* header, hts_idx_t* idx);

  // Our options that control the behavior of this class.
  const nucleus::genomics::v1::SamReaderOptions options_;

  // A pointer to the htslib file used to access the SAM/BAM data.
  htsFile * fp_;

  // A htslib header data structure obtained by parsing the header of this BAM.
  bam_hdr_t * header_;

  // The htslib index data structure for our indexed BAM file. May be NULL if no
  // index was loaded.
  hts_idx_t* idx_;

  // The sam.proto SamHeader message representing the structured header
  // information.
  nucleus::genomics::v1::SamHeader sam_header_;

  // For downsampling reads.
  mutable FractionalSampler sampler_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_SAM_READER_H_
