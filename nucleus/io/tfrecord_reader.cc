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
#include "absl/memory/memory.h"
#include "tensorflow/core/lib/io/record_reader.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

TFRecordReader::TFRecordReader() {}

std::unique_ptr<TFRecordReader> TFRecordReader::New(
    const std::string& filename, const std::string& compression_type) {
  std::unique_ptr<tensorflow::RandomAccessFile> file;
  tensorflow::Status s =
      tensorflow::Env::Default()->NewRandomAccessFile(filename, &file);
  if (!s.ok()) {
    LOG(ERROR) << s.error_message();
    return nullptr;
  }

  auto reader = absl::WrapUnique<TFRecordReader>(new TFRecordReader);
  reader->offset_ = 0;
  reader->file_ = std::move(file);

  tensorflow::io::RecordReaderOptions options =
      tensorflow::io::RecordReaderOptions::CreateRecordReaderOptions(
          compression_type);
  options.buffer_size = 16 * 1024 * 1024;
  reader->reader_ = absl::make_unique<tensorflow::io::RecordReader>(
      reader->file_.get(), options);

  return reader;
}

TFRecordReader::~TFRecordReader() {
}

bool TFRecordReader::GetNext() {
  if (reader_ == nullptr) {
    return false;
  }

  tensorflow::Status s = reader_->ReadRecord(&offset_, &record_);

  return s.ok();
}

void TFRecordReader::Close() {
  reader_ = nullptr;
  file_ = nullptr;
}

}  // namespace nucleus
