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

#include "nucleus/io/gfile.h"

#include "absl/memory/memory.h"
#include "tensorflow/core/lib/io/buffered_inputstream.h"
#include "tensorflow/core/lib/io/random_inputstream.h"
#include "tensorflow/core/platform/env.h"

namespace nucleus {

bool Exists(const std::string& filename) {
  // FileExists sets s to tensorflow::error::NOT_FOUND if it doesn't exist.
  tensorflow::Status s = tensorflow::Env::Default()->FileExists(filename);
  return s.ok();
}

std::vector<std::string> Glob(const std::string& pattern) {
  std::vector<std::string> results;
  tensorflow::Status s = tensorflow::Env::Default()->GetMatchingPaths(
      pattern, &results);
  return results;
}

ReadableFile::ReadableFile() {}

std::unique_ptr<ReadableFile> ReadableFile::New(const std::string& filename) {
  std::unique_ptr<tensorflow::RandomAccessFile> file;
  tensorflow::Status status =
      tensorflow::Env::Default()->NewRandomAccessFile(filename, &file);
  if (!status.ok()) {
    return nullptr;
  }

  size_t buffer_size = 512 * 1024;

  std::unique_ptr<tensorflow::io::RandomAccessInputStream> input_stream(
      new tensorflow::io::RandomAccessInputStream(
          file.release(), true /* owns_file */));
  std::unique_ptr<tensorflow::io::BufferedInputStream> buffered_input_stream(
      new tensorflow::io::BufferedInputStream(
          input_stream.release(), buffer_size, true /* owns_input_stream */));

  auto f = absl::WrapUnique<ReadableFile>(new ReadableFile);
  f->stream_ = std::move(buffered_input_stream);

  return f;
}

ReadableFile::~ReadableFile() {
}

bool ReadableFile::Readline(std::string* s) {
  *s = stream_->ReadLineAsString();
  return s->length() > 0;
}

void ReadableFile::Close() {
  stream_ = nullptr;
}

WritableFile::WritableFile() {}

std::unique_ptr<WritableFile> WritableFile::New(const std::string& filename) {
  std::unique_ptr<tensorflow::WritableFile> file;

  tensorflow::Status s = tensorflow::Env::Default()->NewWritableFile(
      filename, &file);

  if (!s.ok()) {
    return nullptr;
  }

  auto f = absl::WrapUnique<WritableFile>(new WritableFile);
  f->file_ = std::move(file);

  return f;
}

bool WritableFile::Write(const std::string& s) {
  tensorflow::Status status = file_->Append(s);
  return status.ok();
}

void WritableFile::Close() {
  file_ = nullptr;
}

WritableFile::~WritableFile() {
}

}  // namespace nucleus

