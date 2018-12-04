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

#include "nucleus/io/gff_writer.h"

#include <utility>

#include "google/protobuf/text_format.h"
#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/gff.pb.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/vendor/status_matchers.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/env.h"

namespace nucleus {

namespace {

using nucleus::genomics::v1::GffHeader;
using nucleus::genomics::v1::GffRecord;

const char kHeaderRecord[] =
    R"(gff_version: "gff-version 3.2.1"
       sequence_regions {
         reference_name: "ctg123"
         start: 0
         end: 1497228
       }
    )";

const char kGffRecord1[] =
    R"(range {
         reference_name: "ctg123"
         start: 999
         end: 9000
       }
       source: "GenBank"
       type: "gene"
       score: 2.5
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

const char kGffRecord2[] =
    R"(range {
         reference_name: "ctg123"
         start: 999
         end: 1012
       }
       score: -inf
       phase: -1
     )";

const char kExpectedGffText[] =
    "##gff-version 3.2.1\n"
    "##sequence-region ctg123 1 1497228\n"
    "ctg123\tGenBank\tgene\t1000\t9000\t2.5\t+\t0\tID=gene00001;Name=EDEN\n"
    "ctg123\t.\t.\t1000\t1012\t.\t.\t.\t\n";

TEST(GffWriterTest, WritesGffRecords) {
  GffHeader header;
  google::protobuf::TextFormat::ParseFromString(kHeaderRecord, &header);

  GffRecord record1, record2;
  google::protobuf::TextFormat::ParseFromString(kGffRecord1, &record1);
  google::protobuf::TextFormat::ParseFromString(kGffRecord2, &record2);

  string out_fname = MakeTempFile("gff_writer_test_1.gff");
  StatusOr<std::unique_ptr<GffWriter>> writer_or =
      GffWriter::ToFile(out_fname, header);

  ASSERT_THAT(writer_or.status(), IsOK());
  std::unique_ptr<GffWriter> gff_writer = std::move(writer_or.ValueOrDie());

  ASSERT_THAT(gff_writer->Write(record1), IsOK());
  ASSERT_THAT(gff_writer->Write(record2), IsOK());
  ASSERT_THAT(gff_writer->Close(), IsOK());

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           out_fname, &contents));
  EXPECT_EQ(kExpectedGffText, contents);
}

}  // namespace
}  // namespace nucleus
