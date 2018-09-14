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

#ifndef THIRD_PARTY_NUCLEUS_IO_BEDGRAPH_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_BEDGRAPH_READER_H_

#include <memory>
#include <string>

#include "nucleus/io/reader_base.h"
#include "nucleus/io/text_reader.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/bedgraph.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

// Alias for the abstract base class for record iterables.
using BedGraphIterable = Iterable<nucleus::genomics::v1::BedGraphRecord>;

// A BedGraph reader.
//
// BedGraph files stores data values associated with genome sequences in a track
// format.
//
// https://genome.ucsc.edu/goldenpath/help/bedgraph.html
//
// This class provides a method to iterate through a BedGraph file.
//
// The objects returned by iterate() are nucleus.genomics.v1.BedGraphRecord
// objects parsed from the BedGraph records in the file.
//
class BedGraphReader : public Reader {
 public:
  // Creates a new BedGraphReader reading reads from the BedGraph file at
  // |bedgraph_path|.
  //
  // Returns a StatusOr that is OK if the BedGraphReader could be successfully
  // created or an error code indicating the error that occurred.
  static StatusOr<std::unique_ptr<BedGraphReader>> FromFile(
      const string& bedgraph_path);

  ~BedGraphReader();

  // Disable copy and assignment operations.
  BedGraphReader(const BedGraphReader& other) = delete;
  BedGraphReader& operator=(const BedGraphReader&) = delete;

  // Gets all of the BedGraph records in this file in order. Returns an OK
  // status if the iterable can be constructed, or not OK otherwise.
  StatusOr<std::shared_ptr<BedGraphIterable>> Iterate() const;

  // Closes the underlying resource descriptors. Returns a Status to indicate if
  // everything went OK with the close.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.
  void PythonEnter() const {}

 private:
  // Private constructor. Use FromFile to safely create a BedGraphReader from a
  // file.
  BedGraphReader(std::unique_ptr<TextReader> text_reader);

  // A pointer to a raw TextReader object.
  std::unique_ptr<TextReader> text_reader_;

  // Allow BedGraphIterable objects to access fp_.
  friend class BedGraphFullFileIterable;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_BEDGRAPH_READER_H_
