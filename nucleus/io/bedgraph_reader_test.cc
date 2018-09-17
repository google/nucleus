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

#include "nucleus/io/bedgraph_reader.h"

#include <utility>
#include <vector>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/testing/protocol-buffer-matchers.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"

namespace nucleus {

using ::testing::Pointwise;

namespace {
constexpr char kBedGraphFilename[] = "test_regions.bedgraph";
constexpr char kGzippedBedGraphFilename[] = "test_regions.bedgraph.gz";

// Helper to create a BedGraphRecord for testing.
genomics::v1::BedGraphRecord GetTestRecord(const string& name, int64 start,
                                           int64 end, double data_value) {
  nucleus::genomics::v1::BedGraphRecord r;
  r.set_reference_name(name);
  r.set_start(start);
  r.set_end(end);
  r.set_data_value(data_value);
  return r;
}
}  // namespace

TEST(BedGraphReaderTest, NormalIterationWorks) {
  std::vector<genomics::v1::BedGraphRecord> expected = {
      GetTestRecord("chr1", 10, 20, 100), GetTestRecord("chr1", 100, 200, 250),
      GetTestRecord("chr1", 300, 400, 150.1),
      GetTestRecord("chr1", 500, 501, 20.13)};

  std::unique_ptr<BedGraphReader> reader = std::move(
      BedGraphReader::FromFile(GetTestData(kBedGraphFilename)).ValueOrDie());
  EXPECT_THAT(as_vector(reader->Iterate()), Pointwise(EqualsProto(), expected));
}

TEST(BedGraphReaderTest, GzippedIterationWorks) {
  std::unique_ptr<BedGraphReader> reader1 = std::move(
      BedGraphReader::FromFile(GetTestData(kBedGraphFilename)).ValueOrDie());

  std::unique_ptr<BedGraphReader> reader2 =
      std::move(BedGraphReader::FromFile(GetTestData(kGzippedBedGraphFilename))
                    .ValueOrDie());
  EXPECT_THAT(as_vector(reader1->Iterate()),
              Pointwise(EqualsProto(), as_vector(reader2->Iterate())));
}

}  // namespace nucleus
