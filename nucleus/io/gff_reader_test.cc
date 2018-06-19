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
 */

// Tests for GffReader class.

#include "nucleus/io/gff_reader.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"

#include "nucleus/protos/gff.pb.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/testing/protocol-buffer-matchers.h"
#include "nucleus/testing/test_utils.h"

namespace nucleus {
namespace {

using nucleus::genomics::v1::GffHeader;
using nucleus::genomics::v1::GffRecord;

const char kExpectedHeaderRecord[] =
    R"(gff_version: "gff-version 3.2.1"
       sequence_regions {
         reference_name: "ctg123"
         start: 0
         end: 1497228
       }
    )";

const char kExpectedGffRecord1[] =
    R"(range {
         reference_name: "ctg123"
         start: 999
         end: 9000
       }
       source: "GenBank"
       type: "gene"
       score: 2.00
       strand: FORWARD_STRAND
       phase: 0
       attributes {
         key: "ID"
         value: "gene00001"
       }
       attributes {
         key: "Name"
         value: "EDEN"
       }
     )";

const char kExpectedGffRecord2[] =
    R"(range {
         reference_name: "ctg123"
         start: 999
         end: 1012
       }
       score: -inf
       phase: -1
     )";

TEST(GffReaderTest, ReadsExampleFile) {
  string examples_fname = nucleus::GetTestData("test_features.gff");

  auto reader_or = GffReader::FromFile(examples_fname);
  EXPECT_TRUE(reader_or.ok());

  auto reader = std::move(reader_or.ValueOrDie());
  const GffHeader& header = reader->Header();
  EXPECT_THAT(header, EqualsProto(kExpectedHeaderRecord));

  // Load the records.
  std::vector<GffRecord> gff_records = as_vector(reader->Iterate());
  EXPECT_EQ(2, gff_records.size());
  // Inspect the records.
  EXPECT_THAT(gff_records[0], EqualsProto(kExpectedGffRecord1));
  EXPECT_THAT(gff_records[1], EqualsProto(kExpectedGffRecord2));
}

// TODO(dhalexander): Add more comprehensive tests

}  // namespace

}  // namespace nucleus
