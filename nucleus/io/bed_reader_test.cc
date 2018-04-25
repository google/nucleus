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

#include "nucleus/io/bed_reader.h"

#include "nucleus/testing/protocol-buffer-matchers.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"

namespace nucleus {

using std::vector;

using ::testing::Pointwise;

constexpr char kBedFilename[] = "test_regions.bed";
constexpr char kGzippedBedFilename[] = "test_regions.bed.gz";

class BedReaderTest : public ::testing::Test {
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
  }

  vector<nucleus::genomics::v1::BedRecord> golden_;
};

TEST_F(BedReaderTest, NormalIterationWorks) {
  std::unique_ptr<BedReader> reader =
      std::move(BedReader::FromFile(GetTestData(kBedFilename),
                                    nucleus::genomics::v1::BedReaderOptions())
                    .ValueOrDie());

  EXPECT_THAT(as_vector(reader->Iterate()), Pointwise(EqualsProto(), golden_));
}

TEST_F(BedReaderTest, GzippedIterationWorks) {
  auto opts = nucleus::genomics::v1::BedReaderOptions();
  std::unique_ptr<BedReader> reader = std::move(
      BedReader::FromFile(GetTestData(kGzippedBedFilename), opts).ValueOrDie());

  EXPECT_THAT(as_vector(reader->Iterate()), Pointwise(EqualsProto(), golden_));
}

TEST_F(BedReaderTest, FieldRestrictionWorks) {
  auto opts = nucleus::genomics::v1::BedReaderOptions();
  opts.set_num_fields(4);
  std::unique_ptr<BedReader> reader = std::move(
      BedReader::FromFile(GetTestData(kBedFilename), opts).ValueOrDie());

  nucleus::genomics::v1::BedRecord efirst, esecond;
  efirst.set_reference_name("chr1");
  efirst.set_start(10);
  efirst.set_end(20);
  efirst.set_name("first");

  esecond.set_reference_name("chr1");
  esecond.set_start(100);
  esecond.set_end(200);
  esecond.set_name("second");

  vector<nucleus::genomics::v1::BedRecord> expected = {efirst, esecond};

  EXPECT_THAT(as_vector(reader->Iterate()), Pointwise(EqualsProto(), expected));
}
}  // namespace nucleus
