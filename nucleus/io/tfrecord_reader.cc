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

#include "nucleus/io/tfrecord_reader.h"
#include "tensorflow/core/lib/io/record_reader.h"

namespace nucleus {

TFRecordReader::TFRecordReader() {}

TFRecordReader* TFRecordReader::New(const std::string& filename,
                                    const std::string& compression_type)
{
  std::unique_ptr<tensorflow::RandomAccessFile> file;
  tensorflow::Status s =
      tensorflow::Env::Default()->NewRandomAccessFile(filename, &file);
  if (!s.ok()) {
    return nullptr;
  }

  TFRecordReader* reader = new TFRecordReader;
  reader->offset_ = 0;
  reader->file_ = file.release();

  tensorflow::io::RecordReaderOptions options =
      tensorflow::io::RecordReaderOptions::CreateRecordReaderOptions(
          compression_type);
  options.buffer_size = 16 * 1024 * 1024;
  reader->reader_ = new tensorflow::io::RecordReader(reader->file_, options);

  return reader;
}

TFRecordReader::~TFRecordReader() {
  delete reader_;
  delete file_;
}

bool TFRecordReader::GetNext() {
  if (reader_ == nullptr) {
    return false;
  }

  tensorflow::Status s = reader_->ReadRecord(&offset_, &record_);

  return s.ok();
}

void TFRecordReader::Close() {
  delete reader_;
  delete file_;
  file_ = nullptr;
  reader_ = nullptr;
}

}  // namespace nucleus
