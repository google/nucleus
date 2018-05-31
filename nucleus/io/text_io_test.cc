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
#include "nucleus/io/text_reader.h"
#include "nucleus/io/text_writer.h"

#include <fstream>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/platform/types.h"
#include "nucleus/testing/test_utils.h"


namespace tf = tensorflow;


// -----------------------------------------------------------------------------
// Utility functions
namespace {

using nucleus::string;

const string FileContents(const string& path) {
  std::ifstream ifs(path, std::ifstream::binary);
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

string MakeTempFileWithContents(const string& filename,
                                const string& contents) {
  string path = nucleus::MakeTempFile(filename);
  std::ofstream ofs(path, std::ofstream::binary);
  ofs << contents;
  return path;
}

}  // namespace


namespace nucleus {

// -----------------------------------------------------------------------------
// Test data

const char kHelloWorld[] = "Hello, world!";
const string kHelloWorldStr(kHelloWorld);

// Array generated via:
//  printf 'Hello, world!' | bgzip -c | hexdump -v -e '"0x" /1 "%02X" ", "'
const unsigned char kHelloWorldBGZF[] = {
  0x1F, 0x8B, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x06, 0x00,
  0x42, 0x43, 0x02, 0x00, 0x28, 0x00, 0xF3, 0x48, 0xCD, 0xC9, 0xC9, 0xD7,
  0x51, 0x28, 0xCF, 0x2F, 0xCA, 0x49, 0x51, 0x04, 0x00, 0xE6, 0xC6, 0xE6,
  0xEB, 0x0D, 0x00, 0x00, 0x00, 0x1F, 0x8B, 0x08, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x06, 0x00, 0x42, 0x43, 0x02, 0x00, 0x1B, 0x00, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const string kHelloWorldBGZFStr(reinterpret_cast<const char*>(kHelloWorldBGZF),
                                sizeof(kHelloWorldBGZF));


// -----------------------------------------------------------------------------
// TextWriter tests

// Tests that we can write to an uncompressed file stream.
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

// Tests that we can write to a compresed file stream.
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

// Tests that we get compression when filename is *.gz.
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

// Tests that we get  no  compression when filename is NOT *.gz.
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

// -----------------------------------------------------------------------------
// TextReader tests

// Tests that we can read from an uncompressed file stream.
TEST(TextReaderTest, ReadsUncompressedFile) {
  string path =
      MakeTempFileWithContents("uncompressed-for-reader.txt", kHelloWorldStr);
  const auto reader = std::move(TextReader::FromFile(path).ValueOrDie());

  StatusOr<string> rv;
  rv = reader->ReadLine();
  EXPECT_TRUE(rv.ok());
  EXPECT_EQ(rv.ValueOrDie(), kHelloWorldStr);
  rv = reader->ReadLine();
  EXPECT_TRUE(tf::errors::IsOutOfRange(rv.status()));
}


// Tests that we can read from an uncompressed file stream.
TEST(TextReaderTest, ReadsCompressedFile) {
  string path =
      MakeTempFileWithContents("compressed-for-reader.bin", kHelloWorldBGZFStr);
  const auto reader = std::move(TextReader::FromFile(path).ValueOrDie());

  StatusOr<string> rv;
  rv = reader->ReadLine();
  EXPECT_TRUE(rv.ok());
  EXPECT_EQ(rv.ValueOrDie(), kHelloWorldStr);
  rv = reader->ReadLine();
  EXPECT_TRUE(tf::errors::IsOutOfRange(rv.status()));
}


}  // namespace nucleus
