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

#ifndef THIRD_PARTY_NUCLEUS_IO_TFRECORD_WRITER_H_
#define THIRD_PARTY_NUCLEUS_IO_TFRECORD_WRITER_H_

#include <memory>
#include <string>

namespace tensorflow {
class WritableFile;
namespace io {
class RecordWriter;
}  // namespace io
}  // namespace tensorflow

namespace nucleus {

// A class for writing TFRecord files, designed for easy CLIF-wrapping
// for Python.  Loosely based on tensorflow/python/lib/io/py_record_writer.h
// An instance of this class is NOT safe for concurrent access by multiple
// threads.
class TFRecordWriter {
 public:
  // Create a TFRecordWriter.
  // Valid compression_types are "ZLIB", "GZIP", or "" (for none).
  // Returns nullptr on failure.
  static TFRecordWriter* New(const std::string& filename,
                             const std::string& compression_type);

  ~TFRecordWriter();

  // Returns true on success, false on error.
  bool WriteRecord(const std::string& record);

  // Returns true on success, false on error.
  bool Flush();

  // Close the file and release its resources.
  bool Close();

  // Disallow copy and assignment operations.
  TFRecordWriter(const TFRecordWriter& other) = delete;
  TFRecordWriter& operator=(const TFRecordWriter&) = delete;

 private:
  TFRecordWriter();

  std::unique_ptr<tensorflow::io::RecordWriter> writer_;
  std::unique_ptr<tensorflow::WritableFile> file_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_TFRECORD_WRITER_H_
