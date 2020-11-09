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
 *
 */

#include "nucleus/io/text_reader.h"

#include <stdlib.h>
#include <utility>

#include "absl/memory/memory.h"
#include "nucleus/io/hts_path.h"
#include "tensorflow/core/lib/core/errors.h"

namespace tf = tensorflow;

namespace nucleus {

StatusOr<std::unique_ptr<TextReader>> TextReader::FromFile(const string& path) {
  htsFile* fp = hts_open_x(path, "r");

  if (fp == nullptr) {
    return tf::errors::NotFound("Could not open ", path,
                                ". The file might not exist, or the format "
                                "detected by htslib might be incorrect.");
  } else {
    auto reader = absl::WrapUnique(new TextReader(fp));
    return std::move(reader);
  }
}

TextReader::~TextReader() {
  if (hts_file_) {
    TF_CHECK_OK(Close());
  }
}

StatusOr<string> TextReader::ReadLine() {
  tf::Status status;
  string line;
  kstring_t k_line = {0, 0, nullptr};

  int ret = hts_getline(hts_file_, '\n', &k_line);
  if (ret == -1) {
    status = tf::errors::OutOfRange("EOF");
  } else if (ret < 0) {
    status = tf::errors::DataLoss("Failed to read text line");
  }

  if (k_line.s) {
    line = std::string(k_line.s);
    free(k_line.s);
  }
  if (status.ok()) {
    return line;
  } else {
    return status;
  }
}

tf::Status TextReader::Close() {
  if (!hts_file_) {
    return tf::errors::FailedPrecondition(
        "Cannot close an already closed file writer");
  }
  int hts_ok = hts_close(hts_file_);
  hts_file_ = nullptr;
  if (hts_ok < 0) {
    return tf::errors::Internal("hts_close() failed with return code ", hts_ok);
  }
  return tf::Status::OK();
}

TextReader::TextReader(htsFile* hts_file)
    : hts_file_(hts_file) {
  CHECK(hts_file_ != nullptr);
}

}  // namespace nucleus
