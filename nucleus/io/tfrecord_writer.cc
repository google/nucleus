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

#include "nucleus/io/tfrecord_writer.h"
#include "tensorflow/core/lib/io/record_writer.h"

namespace nucleus {

TFRecordWriter::TFRecordWriter() {}

TFRecordWriter* TFRecordWriter::New(const std::string& filename,
                                    const std::string& compression_type)
{
  std::unique_ptr<tensorflow::WritableFile> file;
  tensorflow::Status s =
      tensorflow::Env::Default()->NewWritableFile(filename, &file);
  if (!s.ok()) {
    return nullptr;
  }
  TFRecordWriter* writer = new TFRecordWriter;
  writer->file_ = std::move(file);

  const tensorflow::io::RecordWriterOptions& options =
      tensorflow::io::RecordWriterOptions::CreateRecordWriterOptions(
          compression_type);

  writer->writer_.reset(
      new tensorflow::io::RecordWriter(writer->file_.get(), options));
  return writer;
}

TFRecordWriter::~TFRecordWriter() {
  // Writer depends on file during close for zlib flush, so destruct first.
  writer_.reset();
  file_.reset();
}

bool TFRecordWriter::WriteRecord(const std::string& record) {
  if (writer_ == nullptr) {
    return false;
  }
  tensorflow::Status s = writer_->WriteRecord(record);
  return s.ok();
}

bool TFRecordWriter::Flush() {
  if (writer_ == nullptr) {
    return false;
  }
  tensorflow:: Status s = writer_->Flush();
  return s.ok();
}

bool TFRecordWriter::Close() {
  if (writer_ != nullptr) {
    tensorflow::Status s = writer_->Close();
    if (!s.ok()) {
      return false;
    }
    writer_.reset(nullptr);
  }

  if (file_ != nullptr) {
    tensorflow:: Status s = file_->Close();
    if (!s.ok()) {
      return false;
    }
    file_.reset(nullptr);
  }

  return true;
}

}  // namespace nucleus
