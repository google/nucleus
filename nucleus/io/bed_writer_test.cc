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

#include "nucleus/io/bed_writer.h"

#include <utility>
#include <vector>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/env.h"

namespace nucleus {

using std::vector;

class BedWriterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    nucleus::genomics::v1::BedRecord first, second;
    first.set_reference_name("chr1");
    first.set_start(10);
    first.set_end(20);
    first.set_name("first");
    first.set_score(100);
    first.set_strand(nucleus::genomics::v1::BedRecord::FORWARD_STRAND);
    first.set_thick_start(12);
    first.set_thick_end(18);
    first.set_item_rgb("255,124,1");
    first.set_block_count(3);
    first.set_block_sizes("2,6,2");
    first.set_block_starts("10,12,18");

    second.set_reference_name("chr1");
    second.set_start(100);
    second.set_end(200);
    second.set_name("second");
    second.set_score(250);
    second.set_strand(nucleus::genomics::v1::BedRecord::NO_STRAND);
    second.set_thick_start(120);
    second.set_thick_end(180);
    second.set_item_rgb("252,122,12");
    second.set_block_count(2);
    second.set_block_sizes("35,40");
    second.set_block_starts("100,160");

    golden_ = {first, second};
    header_.set_num_fields(12);
  }

  nucleus::genomics::v1::BedHeader header_;
  vector<nucleus::genomics::v1::BedRecord> golden_;
};

TEST_F(BedWriterTest, WritingWorks) {
  const string output_filename = MakeTempFile("writes_bed.bed");
  std::unique_ptr<BedWriter> writer =
      std::move(BedWriter::ToFile(output_filename, header_,
                                  nucleus::genomics::v1::BedWriterOptions())
                    .ValueOrDie());
  for (const nucleus::genomics::v1::BedRecord& record : golden_) {
    ASSERT_THAT(writer->Write(record), IsOK());
  }
  ASSERT_THAT(writer->Close(), IsOK());
  writer.reset();

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  const string kExpectedBedContent =
      "chr1\t10\t20\tfirst\t100\t+\t12\t18\t255,124,1\t3\t2,6,2\t10,12,18\n"
      "chr1\t100\t200\tsecond\t250\t.\t120\t180\t252,122,12\t2\t35,40\t100,"
      "160\n";
  EXPECT_EQ(kExpectedBedContent, contents);
}

TEST_F(BedWriterTest, WritesGzippedFiles) {
  const string output_filename = MakeTempFile("writes_bed.bed.gz");
  std::unique_ptr<BedWriter> writer =
      std::move(BedWriter::ToFile(output_filename, header_,
                                  nucleus::genomics::v1::BedWriterOptions())
                    .ValueOrDie());
  ASSERT_THAT(writer->Close(), IsOK());

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  EXPECT_THAT(IsGzipped(contents),
              "BED writer should be able to writed gzipped output");
}

TEST_F(BedWriterTest, WritingTruncatedWorks) {
  const string output_filename = MakeTempFile("writes_short_bed.bed");
  nucleus::genomics::v1::BedHeader truncated_header;
  truncated_header.set_num_fields(6);
  std::unique_ptr<BedWriter> writer =
      std::move(BedWriter::ToFile(output_filename, truncated_header,
                                  nucleus::genomics::v1::BedWriterOptions())
                    .ValueOrDie());
  for (const nucleus::genomics::v1::BedRecord& record : golden_) {
    ASSERT_THAT(writer->Write(record), IsOK());
  }
  ASSERT_THAT(writer->Close(), IsOK());
  writer.reset();

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  const string kExpectedBedContent =
      "chr1\t10\t20\tfirst\t100\t+\n"
      "chr1\t100\t200\tsecond\t250\t.\n";
  EXPECT_EQ(kExpectedBedContent, contents);
}
}  // namespace nucleus
