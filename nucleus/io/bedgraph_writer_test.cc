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

#include "nucleus/io/bedgraph_writer.h"

#include <utility>
#include <vector>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/io/bedgraph_reader.h"
#include "nucleus/testing/protocol-buffer-matchers.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/env.h"

namespace nucleus {

using genomics::v1::BedGraphRecord;

namespace {

constexpr char kBedGraphFilename[] = "test_regions.bedgraph";

// Helper to create a BedGraphRecord for testing.
BedGraphRecord MakeTestRecord(const string& name, int64 start, int64 end,
                              double data_value) {
  nucleus::genomics::v1::BedGraphRecord r;
  r.set_reference_name(name);
  r.set_start(start);
  r.set_end(end);
  r.set_data_value(data_value);
  return r;
}

}  // namespace

TEST(BedGraphWriterTest, Writes) {
  std::vector<BedGraphRecord> expected = {
      MakeTestRecord("chr1", 10, 20, 100.1),
      MakeTestRecord("chr1", 100, 200, 250.50),
      MakeTestRecord("chr1", 300, 320, 25.13)};
  const string output_filename = MakeTempFile("writes.bedgraph");
  std::unique_ptr<BedGraphWriter> writer =
      std::move(BedGraphWriter::ToFile(output_filename).ValueOrDie());
  for (const BedGraphRecord& record : expected) {
    ASSERT_THAT(writer->Write(record), IsOK());
  }
  ASSERT_THAT(writer->Close(), IsOK());
  writer.reset();

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  const string kExpectedBedContent =
      "chr1\t10\t20\t100.1\n"
      "chr1\t100\t200\t250.5\n"
      "chr1\t300\t320\t25.13\n";
  EXPECT_EQ(kExpectedBedContent, contents);
  TF_CHECK_OK(tensorflow::Env::Default()->DeleteFile(output_filename));
}

TEST(BedGraphWriterTest, WritesGzippedFiles) {
  std::vector<BedGraphRecord> expected = {
      MakeTestRecord("chr1", 10, 20, 100.1),
      MakeTestRecord("chr1", 100, 200, 250.50),
      MakeTestRecord("chr1", 300, 320, 25.13)};
  const string output_filename = MakeTempFile("writes.bedgraph.gz");
  std::unique_ptr<BedGraphWriter> writer =
      std::move(BedGraphWriter::ToFile(output_filename).ValueOrDie());
  for (const BedGraphRecord& record : expected) {
    ASSERT_THAT(writer->Write(record), IsOK());
  }
  ASSERT_THAT(writer->Close(), IsOK());

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  EXPECT_THAT(IsGzipped(contents),
              "BED writer should be able to writed gzipped output");
  TF_CHECK_OK(tensorflow::Env::Default()->DeleteFile(output_filename));
}

TEST(BedGraphWriterTest, RoundTrip) {
  std::unique_ptr<BedGraphReader> reader = std::move(
      BedGraphReader::FromFile(GetTestData(kBedGraphFilename)).ValueOrDie());
  const string output_filename = MakeTempFile("writes.bedgraph");
  std::vector<BedGraphRecord> expected = as_vector(reader->Iterate());
  std::unique_ptr<BedGraphWriter> writer =
      std::move(BedGraphWriter::ToFile(output_filename).ValueOrDie());
  for (const BedGraphRecord& record : expected) {
    ASSERT_THAT(writer->Write(record), IsOK());
  }
  ASSERT_THAT(writer->Close(), IsOK());
  writer.reset();

  std::unique_ptr<BedGraphReader> reader2 =
      std::move(BedGraphReader::FromFile(output_filename).ValueOrDie());

  std::vector<BedGraphRecord> actual = as_vector(reader2->Iterate());

  EXPECT_THAT(actual, ::testing::Pointwise(EqualsProto(), expected));
  TF_CHECK_OK(tensorflow::Env::Default()->DeleteFile(output_filename));
}

}  // namespace nucleus
