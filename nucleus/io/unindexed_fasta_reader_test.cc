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

#include "nucleus/io/unindexed_fasta_reader.h"

#include <utility>

#include "tensorflow/core/platform/test.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

TEST(UnindexedFastaReaderTest, ReturnsBadStatusIfFileIsMissing) {
  StatusOr<std::unique_ptr<UnindexedFastaReader>> result =
      UnindexedFastaReader::FromFile(GetTestData("nonexistent.fasta"));
  EXPECT_THAT(result, IsNotOKWithCodeAndMessage(tensorflow::error::NOT_FOUND,
                                                "Could not open"));
}

TEST(UnindexedFastaReaderTest, IterateAfterCloseIsntOK) {
  StatusOr<std::unique_ptr<UnindexedFastaReader>> result =
      UnindexedFastaReader::FromFile(GetTestData("unindexed.fasta"));
  auto reader = std::move(result.ValueOrDie());
  auto iterator = reader->Iterate().ValueOrDie();
  ASSERT_THAT(reader->Close(), IsOK());
  GenomeReferenceRecord r;
  StatusOr<bool> status = iterator->Next(&r);
  EXPECT_THAT(iterator->Next(&r),
              IsNotOKWithCodeAndMessage(
                  tensorflow::error::FAILED_PRECONDITION,
                  "Cannot iterate a closed UnindexedFastaReader"));
}

TEST(UnindexedFastaReaderTest, TestMalformed) {
  StatusOr<std::unique_ptr<UnindexedFastaReader>> result =
      UnindexedFastaReader::FromFile(GetTestData("malformed.fasta"));
  auto reader = std::move(result.ValueOrDie());
  auto iterator = reader->Iterate().ValueOrDie();
  GenomeReferenceRecord r;
  EXPECT_THAT(iterator->Next(&r),
              IsNotOKWithCodeAndMessage(tensorflow::error::DATA_LOSS,
                                        "Name not found in FASTA"));
}

class UnindexedFastaReaderFileTest : public ::testing::TestWithParam<string> {};

// Test a couple of files that are formatted differently but should have the
// same contents.
INSTANTIATE_TEST_CASE_P(/* prefix */, UnindexedFastaReaderFileTest,
                        ::testing::Values("unindexed.fasta", "test.fasta.gz",
                                          "unindexed_emptylines.fasta"));

TEST_P(UnindexedFastaReaderFileTest, TestIterate) {
  LOG(INFO) << "testing file " << GetParam();
  StatusOr<std::unique_ptr<UnindexedFastaReader>> result =
      UnindexedFastaReader::FromFile(GetTestData(GetParam()));
  auto reader = std::move(result.ValueOrDie());
  auto iterator = reader->Iterate().ValueOrDie();
  GenomeReferenceRecord r1;
  StatusOr<bool> status = iterator->Next(&r1);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("chrM", r1.first);
  EXPECT_EQ(
      "GATCACAGGTCTATCACCCTATTAACCACTCACGGGAGCTCTCCATGCATTTGGTATTTTCGTCTGGGGGGT"
      "GTGCACGCGATAGCATTGCGAGACGCTG",
      r1.second);

  GenomeReferenceRecord r2;
  status = iterator->Next(&r2);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("chr1", r2.first);
  EXPECT_EQ(
      "ACCACCATCCTCCGTGAAATCAATATCCCGCACAAGAGTGCTACTCTCCTAAATCCCTTCTCGTCCCCATGG"
      "ATGA",
      r2.second);
  GenomeReferenceRecord r3;
  status = iterator->Next(&r3);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("chr2", r3.first);
  EXPECT_EQ(
      "CGCTNCGGGCCCATAACACTTGGGGGTAGCTAAAGTGAACTGTATCCGACATCTGGTTCCTACTTCAGGGCC"
      "ATAAAGCCTAAATAGCCCACACGTTCCCCTTAAATAAGACATCACGATG",
      r3.second);

  // Reading beyond the file fails.
  GenomeReferenceRecord r4;
  status = iterator->Next(&r4);
  EXPECT_FALSE(status.ValueOrDie());
}

}  // namespace nucleus
