/*
 * Copyright 2019 Google LLC.
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

#ifndef THIRD_PARTY_NUCLEUS_IO_TFRECORD_READER_H_
#define THIRD_PARTY_NUCLEUS_IO_TFRECORD_READER_H_

#include <memory>
#include <string>

#include "nucleus/platform/types.h"
#include "tensorflow/core/platform/tstring.h"
#include "tensorflow/core/platform/types.h"

namespace tensorflow {
class RandomAccessFile;
namespace io {
class RecordReader;
}  // namespace io
}  // namespace tensorflow

namespace nucleus {

// A class for reading TFRecord files, designed for easy CLIF-wrapping
// for Python.  Loosely based on tensorflow/python/lib/io/py_record_reader.h
// An instance of this class is NOT safe for concurrent access by multiple
// threads.
class TFRecordReader {
 public:
  // Create a TFRecordReader.
  // Valid compression_types are "ZLIB", "GZIP", or "" (for none).
  // Returns nullptr on failure.
  static std::unique_ptr<TFRecordReader> New(
      const std::string& filename, const std::string& compression_type);

  ~TFRecordReader();

  // Returns true on success, false on error.
  bool GetNext();

  // Return the current record contents.  Only valid after GetNext()
  // has returned true.
  tensorflow::tstring record() const { return record_; }

  // Close the file and release its resources.
  void Close();

  // Disallow copy and assignment operations.
  TFRecordReader(const TFRecordReader& other) = delete;
  TFRecordReader& operator=(const TFRecordReader&) = delete;

 private:
  TFRecordReader();

  tensorflow::uint64 offset_;

  // |reader_| has a non-owning pointer on |file_|, so destruct it first.
  std::unique_ptr<tensorflow::RandomAccessFile> file_;
  std::unique_ptr<tensorflow::io::RecordReader> reader_;

  tensorflow::tstring record_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_TFRECORD_READER_H_
