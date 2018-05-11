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

#include "nucleus/io/fastq_writer.h"

#include <vector>

#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"

#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/core/stringpiece.h"

#include "tensorflow/core/platform/test.h"

namespace nucleus {

using std::vector;

void CreateRecord(const string& id, const string& description,
                  const string& sequence, const string& quality,
                  nucleus::genomics::v1::FastqRecord* record) {
  record->set_id(id);
  record->set_description(description);
  record->set_sequence(sequence);
  record->set_quality(quality);
}

bool IsGzipped(const StringPiece& input) {
  const char gzip_magic[2] = {'\x1f', '\x8b'};
  return (input.size() >= 2 && input[0] == gzip_magic[0] &&
          input[1] == gzip_magic[1]);
}

class FastqWriterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    nucleus::genomics::v1::FastqRecord first, second, third;
    CreateRecord("NODESC:header", "", "GATTACA", "BB>B@FA", &first);
    CreateRecord(
        "M01321:49:000000000-A6HWP:1:1101:17009:2216", "1:N:0:1",
        "CGTTAGCGCAGGGGGCATCTTCACACTGGTGACAGGTAACCGCCGTAGTAAAGGTTCCGCCTTTCACT",
        "AAAAABF@BBBDGGGG?FFGFGHBFBFBFABBBHGGGFHHCEFGGGGG?FGFFHEDG3EFGGGHEGHG",
        &second);
    CreateRecord("FASTQ", "contains multiple spaces in description",
                 "CGGCTGGTCAGGCTGACATCGCCGCCGGCCTGCAGCGAGCCGCTGC",
                 "FAFAF;F/9;.:/;999B/9A.DFFF;-->.AAB/FC;9-@-=;=.", &third);
    golden_ = {first, second, third};
  }

  vector<nucleus::genomics::v1::FastqRecord> golden_;
};

TEST_F(FastqWriterTest, WritingWorks) {
  const string output_filename = MakeTempFile("writes_fastq.fastq");
  std::unique_ptr<FastqWriter> writer =
      std::move(FastqWriter::ToFile(output_filename,
                                    nucleus::genomics::v1::FastqWriterOptions())
                    .ValueOrDie());
  for (const nucleus::genomics::v1::FastqRecord& record : golden_) {
    ASSERT_THAT(writer->Write(record), IsOK());
  }
  ASSERT_THAT(writer->Close(), IsOK());
  writer.reset();

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  const string kExpectedFastqContent =
      "@NODESC:header\n"
      "GATTACA\n"
      "+\n"
      "BB>B@FA\n"
      "@M01321:49:000000000-A6HWP:1:1101:17009:2216 1:N:0:1\n"
      "CGTTAGCGCAGGGGGCATCTTCACACTGGTGACAGGTAACCGCCGTAGTAAAGGTTCCGCCTTTCACT\n"
      "+\n"
      "AAAAABF@BBBDGGGG?FFGFGHBFBFBFABBBHGGGFHHCEFGGGGG?FGFFHEDG3EFGGGHEGHG\n"
      "@FASTQ contains multiple spaces in description\n"
      "CGGCTGGTCAGGCTGACATCGCCGCCGGCCTGCAGCGAGCCGCTGC\n"
      "+\n"
      "FAFAF;F/9;.:/;999B/9A.DFFF;-->.AAB/FC;9-@-=;=.\n";
  EXPECT_EQ(kExpectedFastqContent, contents);
}

TEST_F(FastqWriterTest, WritesGzippedFiles) {
  const string output_filename = MakeTempFile("writes_fastq.fastq.gz");
  std::unique_ptr<FastqWriter> writer =
      std::move(FastqWriter::ToFile(output_filename,
                                    nucleus::genomics::v1::FastqWriterOptions())
                    .ValueOrDie());
  ASSERT_THAT(writer->Close(), IsOK());

  string contents;
  TF_CHECK_OK(tensorflow::ReadFileToString(tensorflow::Env::Default(),
                                           output_filename, &contents));
  EXPECT_THAT(IsGzipped(contents),
              "FASTQ writer should be able to writed gzipped output");
}
}  // namespace nucleus
