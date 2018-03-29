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

#ifndef THIRD_PARTY_NUCLEUS_IO_FASTQ_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_FASTQ_READER_H_

#include "nucleus/io/reader_base.h"
#include "nucleus/protos/fastq.pb.h"
#include "nucleus/vendor/statusor.h"
#include "nucleus/vendor/zlib_inputstream.h"
#include "tensorflow/core/lib/io/buffered_inputstream.h"
#include "tensorflow/core/lib/io/random_inputstream.h"
#include "tensorflow/core/platform/file_system.h"
#include "tensorflow/core/platform/types.h"

namespace nucleus {

using tensorflow::string;

// Alias for the abstract base class for FASTQ record iterables.
using FastqIterable = Iterable<nucleus::genomics::v1::FastqRecord>;

// A FASTQ reader.
//
// FASTQ files store information about a biological sequence and its
// corresponding quality scores.
//
// https://en.wikipedia.org/wiki/FASTQ_format
//
// This class provides a method to iterate through a FASTQ file.
//
// The objects returned by iterate() are nucleus.genomics.v1.FastqRecord
// objects parsed from the FASTQ records in the file.
//
class FastqReader : public Reader {
 public:
  // Creates a new FastqReader reading reads from the FASTQ file fastq_path.
  //
  // fastq_path must point to an existing FASTQ formatted file.
  //
  // Returns a StatusOr that is OK if the FastqReader could be successfully
  // created or an error code indicating the error that occurred.
  static StatusOr<std::unique_ptr<FastqReader>> FromFile(
      const string& fastq_path,
      const nucleus::genomics::v1::FastqReaderOptions& options);

  ~FastqReader();

  // Disable copy and assignment operations.
  FastqReader(const FastqReader& other) = delete;
  FastqReader& operator=(const FastqReader&) = delete;

  // Gets all of the FASTQ records in this file in order.
  //
  // The specific parsing, filtering, etc behavior is determined by the options
  // provided during construction. Returns an OK status if the iterable can be
  // constructed, or not OK otherwise.
  StatusOr<std::shared_ptr<FastqIterable>> Iterate() const;

  // Close the underlying resource descriptors. Returns a Status to indicate if
  // everything went OK with the close.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.  Do
  // not use it! Returns a Status indicating whether the enter was successful.
  tensorflow::Status PythonEnter() const { return tensorflow::Status::OK(); }

  // Get the options controlling the behavior of this FastqReader.
  const nucleus::genomics::v1::FastqReaderOptions& Options() const {
    return options_;
  }

  // Populates the three string pointers with values from the input stream.
  tensorflow::Status Next(string* header, string* sequence, string* pad,
                          string* quality) const;

 private:
  // Private constructor; use FromFile to safely create a FastqReader from a
  // file.
  FastqReader(tensorflow::RandomAccessFile* fp,
              const nucleus::genomics::v1::FastqReaderOptions& options);

  // Our options that control the behavior of this class.
  const nucleus::genomics::v1::FastqReaderOptions options_;

  // The file pointer for the given FASTQ path. The FastqReader owns its file
  // pointer and is responsible for its deletion.
  tensorflow::RandomAccessFile* src_;
  // Must outlive buffered_inputstream_.
  std::unique_ptr<tensorflow::io::RandomAccessInputStream> file_stream_;
  // Must outlive buffered_inputstream_.
  std::unique_ptr<tensorflow::io::ZlibInputStream> zlib_stream_;
  std::unique_ptr<tensorflow::io::BufferedInputStream> buffered_inputstream_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_FASTQ_READER_H_
