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

#ifndef THIRD_PARTY_NUCLEUS_IO_GFF_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_GFF_READER_H_

#include <memory>
#include <string>

#include "nucleus/io/reader_base.h"
#include "nucleus/io/text_reader.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/gff.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

const nucleus::genomics::v1::GffReaderOptions kDefaultGffReaderOptions{};

// Alias for the abstract base class for GFF record iterables.
using GffIterable = Iterable<nucleus::genomics::v1::GffRecord>;

class GffReader : public Reader {
 public:
  // Creates a new GffReader reading reads from the GFF file gff_path.
  //
  // gff_path must point to an existing GFF formatted file (or gzipped
  // equivalent).
  //
  // The GFF format is described here:
  // https://github.com/The-Sequence-Ontology/Specifications/blob/master/gff3.md
  //
  // Returns a StatusOr that is OK if the GffReader could be successfully
  // created or an error code indicating the error that occurred.
  static StatusOr<std::unique_ptr<GffReader>> FromFile(
      const string& gff_path,
      const nucleus::genomics::v1::GffReaderOptions& options =
          kDefaultGffReaderOptions);

  ~GffReader() = default;

  // Disable copy and assignment operations.
  GffReader(const GffReader& other) = delete;
  GffReader& operator=(const GffReader&) = delete;

  // Gets all of the GFF records in this file in order.
  // Returns an OK status if the iterable can be constructed, or not
  // OK otherwise.  Iteration is over proto records of type
  // nucleus.genomics.v1.GffRecord
  StatusOr<std::shared_ptr<GffIterable>> Iterate() const;

  // Closes the underlying resource descriptors. Returns a Status to
  // indicate if everything went OK with the close.
  tensorflow::Status Close();

  // This no-op function is needed only for Python context manager support.
  void PythonEnter() const {}

  // Get the options controlling the behavior of this GffReader.
  const nucleus::genomics::v1::GffReaderOptions& Options() const {
    return options_;
  }

  // Returns the header that tracks the number of fields in each record in the
  // reader.
  const nucleus::genomics::v1::GffHeader& Header() const { return header_; }

 private:
  // Private constructor used by FromFile factory.
  GffReader(std::unique_ptr<TextReader> text_reader,
            const nucleus::genomics::v1::GffReaderOptions& options,
            const nucleus::genomics::v1::GffHeader& header);

  // A pointer to a raw TextReader object.
  std::unique_ptr<TextReader> text_reader_;

  // Options controlling the behavior of this class.
  const nucleus::genomics::v1::GffReaderOptions options_;

  // The GFF header, reflecting how to interpret fields.
  const nucleus::genomics::v1::GffHeader header_;

  // Allow iteration to access the underlying reader.
  friend class GffFullFileIterable;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_GFF_READER_H_
