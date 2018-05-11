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

#ifndef THIRD_PARTY_NUCLEUS_IO_BED_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_BED_READER_H_

#include "nucleus/io/reader_base.h"
#include "nucleus/io/text_reader.h"
#include "nucleus/protos/bed.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/platform/types.h"

namespace nucleus {

using tensorflow::string;

// Alias for the abstract base class for BED record iterables.
using BedIterable = Iterable<nucleus::genomics::v1::BedRecord>;

// A BED reader.
//
// BED files are flexible stores of information about a genome annotation track.
//
// https://genome.ucsc.edu/FAQ/FAQformat.html#format1
//
// This class provides a method to iterate through a BED file.
//
// The objects returned by iterate() are nucleus.genomics.v1.BedRecord
// objects parsed from the BED records in the file.
//
// Note: Only tab-delimited BED files are supported, for ease of future support
// for tabix-indexed BED file querying.
//
class BedReader : public Reader {
 public:
  // Creates a new BedReader reading reads from the BED file bed_path.
  //
  // bed_path must point to an existing BED formatted file.
  //
  // Returns a StatusOr that is OK if the BedReader could be successfully
  // created or an error code indicating the error that occurred.
  static StatusOr<std::unique_ptr<BedReader>> FromFile(
      const string& bed_path,
      const nucleus::genomics::v1::BedReaderOptions& options);

  ~BedReader();

  // Disable copy and assignment operations.
  BedReader(const BedReader& other) = delete;
  BedReader& operator=(const BedReader&) = delete;

  // Gets all of the BED records in this file in order.
  //
  // The specific parsing, filtering, etc behavior is determined by the options
  // provided during construction. Returns an OK status if the iterable can be
  // constructed, or not OK otherwise.
  StatusOr<std::shared_ptr<BedIterable>> Iterate() const;

  // Close the underlying resource descriptors. Returns a Status to indicate if
  // everything went OK with the close.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.
  void PythonEnter() const {}

  // Get the options controlling the behavior of this BedReader.
  const nucleus::genomics::v1::BedReaderOptions& Options() const {
    return options_;
  }

  // Returns the header that tracks the number of fields in each record in the
  // reader.
  const nucleus::genomics::v1::BedHeader& Header() const { return header_; }

  // Returns OK if the input numTokens equals num_fields in the header.
  tensorflow::Status Validate(const int numTokens) const;

 private:
  // Private constructor; use FromFile to safely create a BedReader from a
  // file.
  BedReader(std::unique_ptr<TextReader> text_reader,
            const nucleus::genomics::v1::BedReaderOptions& options,
            const nucleus::genomics::v1::BedHeader& header);

  // Our options that control the behavior of this class.
  const nucleus::genomics::v1::BedReaderOptions options_;

  // The header that tracks the number of fields in each record in the file.
  const nucleus::genomics::v1::BedHeader header_;

  // A pointer to a raw TextReader object.
  std::unique_ptr<TextReader> text_reader_;

  // Allow BedIterable objects to access fp_.
  friend class BedFullFileIterable;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_BED_READER_H_
