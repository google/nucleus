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

namespace {

using nucleus::EqualsProto;
using nucleus::GffReader;
using nucleus::genomics::v1::GffHeader;
using nucleus::genomics::v1::GffRecord;
using nucleus::string;


TEST(GffReaderTest, ReadsExampleFile) {
  string examples_fname = nucleus::GetTestData("test_features.gff");

  auto reader_or = GffReader::FromFile(examples_fname);
  EXPECT_TRUE(reader_or.ok());

  auto reader = std::move(reader_or.ValueOrDie());
  const GffHeader& header = reader->Header();

  EXPECT_EQ("gff-version 3.2.1", header.gff_version());

  EXPECT_EQ(1, header.sequence_regions_size());
  EXPECT_THAT(header.sequence_regions(0),
              EqualsProto(R"(reference_name: "ctg123" start: 0 end: 1497228)"));

  // Load the records.
  std::vector<GffRecord> gff_records = as_vector(reader->Iterate());
  EXPECT_EQ(23, gff_records.size());

  // Inspect the first record.
  // TODO(dhalexander): check for attributes.
  EXPECT_THAT(gff_records[0], EqualsProto(
    R"(range {
         reference_name: "ctg123"
         start: 999
         end: 9000
       }
       source: "."
       type: "gene"
       strand: FORWARD_STRAND
     )"));
}

// TODO(dhalexander): Add more comprehensive tests

}  // namespace
