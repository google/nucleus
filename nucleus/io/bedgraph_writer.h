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

#ifndef THIRD_PARTY_NUCLEUS_IO_BEDGRAPH_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_BEDGRAPH_WRITER_H_

#include <memory>
#include <string>

#include "nucleus/io/text_writer.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/bedgraph.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

// A BedGraph writer.
//
// BedGraph files stores data values associated with genome sequences in a track
// format.
//
// https://genome.ucsc.edu/goldenpath/help/bedgraph.html
//
// This class allows writing a BedGraph file using
// nucleus.genomics.v1.BedGraphRecord objects.
class BedGraphWriter {
 public:
  // Creates a new BedGraphWriter writing to the file at |bedgraph_path|, which
  // is opened and created if needed. Returns either a unique_ptr to the
  // BedGraphWriter or a Status indicating why an error occurred.
  static StatusOr<std::unique_ptr<BedGraphWriter>> ToFile(
      const string& bedgraph_path);

  ~BedGraphWriter();

  // Disables copy and assignment operations.
  BedGraphWriter(const BedGraphWriter& other) = delete;
  BedGraphWriter& operator=(const BedGraphWriter&) = delete;

  // Writes a BedGraphRecord to the Bedgraph file.
  // Returns Status::OK() if the write was successful; otherwise the status
  // provides information about why an error occurred.
  tensorflow::Status Write(const nucleus::genomics::v1::BedGraphRecord& record);

  // Close the underlying resource descriptors. Returns Status::OK() if the
  // close was successful; otherwise the status provides information about what
  // error occurred.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support. Do
  // not use it.
  void PythonEnter() const {}

 private:
  // Private constructor. Use ToFile to safely create a BedGraphWriter.
  BedGraphWriter(std::unique_ptr<TextWriter> text_writer);

  // Underlying file writer.
  std::unique_ptr<TextWriter> text_writer_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_BedgraphGRAPH_WRITER_H_
