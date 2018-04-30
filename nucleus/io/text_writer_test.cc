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

#include "nucleus/io/text_writer.h"

#include <fstream>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/testing/test_utils.h"

using tensorflow::string;

namespace tf = tensorflow;

namespace {

const string FileContents(const string& path) {
  std::ifstream ifs(path, std::ifstream::binary);
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

}  // namespace


namespace nucleus {

const char kHelloWorld[] = "Hello, world!\n";
const string kHelloWorldStr(kHelloWorld);

// Array generated via:
//  printf 'Hello, world!\n' | bgzip -c | hexdump -v -e '"0x" /1 "%02X" ", "'
const unsigned char kHelloWorldBGZF[] = {
  0x1F, 0x8B, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x06, 0x00,
  0x42, 0x43, 0x02, 0x00, 0x29, 0x00, 0xF3, 0x48, 0xCD, 0xC9, 0xC9, 0xD7,
  0x51, 0x28, 0xCF, 0x2F, 0xCA, 0x49, 0x51, 0xE4, 0x02, 0x00, 0x18, 0xA7,
  0x55, 0x7B, 0x0E, 0x00, 0x00, 0x00, 0x1F, 0x8B, 0x08, 0x04, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0x06, 0x00, 0x42, 0x43, 0x02, 0x00, 0x1B, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const string kHelloWorldBGZFStr(reinterpret_cast<const char*>(kHelloWorldBGZF),
                                sizeof(kHelloWorldBGZF));



// Test that we can write to an uncompressed file stream.
TEST(TextWriterTest, WritesUncompressedOutput) {
  string dest = MakeTempFile("uncompressed.txt");
  const auto writer = std::move(
      TextWriter::ToFile(dest, TextWriter::NO_COMPRESS).ValueOrDie());
  tf::Status status;
  status = writer->Write(kHelloWorld);
  EXPECT_EQ(tf::Status::OK(), status);
  status = writer->Close();
  EXPECT_EQ(tf::Status::OK(), status);

  EXPECT_EQ(kHelloWorldStr, FileContents(dest));
}

// Test that we can write to a compresed file stream.
TEST(TextWriterTest, WritesCompressedOutput) {
  string dest = MakeTempFile("compressed.txt.gz");
  const auto writer = std::move(
      TextWriter::ToFile(dest, TextWriter::COMPRESS).ValueOrDie());
  tf::Status status;
  status = writer->Write(kHelloWorld);
  EXPECT_EQ(tf::Status::OK(), status);
  status = writer->Close();
  EXPECT_EQ(tf::Status::OK(), status);

  EXPECT_EQ(kHelloWorldBGZFStr, FileContents(dest));
}

// Test that we get compression when filename is *.gz.
TEST(TextWriterTest, UsesCompressionWhenExtensionIsGz) {
  string destGz = MakeTempFile("test_file.txt.gz");
  const auto writer = std::move(TextWriter::ToFile(destGz).ValueOrDie());
  tf::Status status;
  status = writer->Write(kHelloWorld);
  EXPECT_EQ(tf::Status::OK(), status);
  status = writer->Close();
  EXPECT_EQ(tf::Status::OK(), status);

  EXPECT_EQ(kHelloWorldBGZFStr, FileContents(destGz));
}

// And no  compression when filename is NOT *.gz.
TEST(TextWriterTest, NoCompressionWhenExtensionIsNotGz) {
  string dest = MakeTempFile("test_file.txt");
  const auto writer = std::move(TextWriter::ToFile(dest).ValueOrDie());
  tf::Status status;
  status = writer->Write(kHelloWorld);
  EXPECT_EQ(tf::Status::OK(), status);
  status = writer->Close();
  EXPECT_EQ(tf::Status::OK(), status);

  EXPECT_EQ(kHelloWorld, FileContents(dest));
}






}  // namespace nucleus
