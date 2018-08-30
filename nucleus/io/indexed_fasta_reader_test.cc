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

#include "nucleus/io/indexed_fasta_reader.h"

#include <memory>
#include <utility>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "absl/strings/str_cat.h"
#include "nucleus/io/reader_base.h"
#include "nucleus/io/reference_test.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

using absl::StrCat;
using std::make_pair;
using testing::Eq;
using testing::StartsWith;

namespace nucleus {

static std::unique_ptr<GenomeReference> JustLoadFai(const string& fasta,
                                                    int cache_size = 64 *
                                                                     1024) {
  StatusOr<std::unique_ptr<IndexedFastaReader>> fai_status =
      IndexedFastaReader::FromFile(fasta, StrCat(fasta, ".fai"), cache_size);
  TF_CHECK_OK(fai_status.status());
  return std::move(fai_status.ValueOrDie());
}

// Test with cache disabled.
INSTANTIATE_TEST_CASE_P(GRT1, GenomeReferenceTest,
                        ::testing::Values(make_pair(&JustLoadFai, 0)));

INSTANTIATE_TEST_CASE_P(GRT2, GenomeReferenceDeathTest,
                        ::testing::Values(make_pair(&JustLoadFai, 0)));

// Test with a large cache.
INSTANTIATE_TEST_CASE_P(GRT3, GenomeReferenceTest,
                        ::testing::Values(make_pair(&JustLoadFai, 64 * 1024)));

INSTANTIATE_TEST_CASE_P(GRT4, GenomeReferenceDeathTest,
                        ::testing::Values(make_pair(&JustLoadFai, 64 * 1024)));

TEST(StatusOrLoadFromFile, ReturnsBadStatusIfFaiIsMissing) {
  StatusOr<std::unique_ptr<IndexedFastaReader>> result =
      IndexedFastaReader::FromFile(GetTestData("unindexed.fasta"),
                                   GetTestData("unindexed.fasta.fai"));
  EXPECT_THAT(result, IsNotOKWithCodeAndMessage(
                          tensorflow::error::NOT_FOUND,
                          "could not load fasta and/or fai for fasta"));
}

TEST(IndexedFastaReaderTest, WriteAfterCloseIsntOK) {
  auto reader = JustLoadFai(TestFastaPath());
  ASSERT_THAT(reader->Close(), IsOK());
  EXPECT_THAT(reader->GetBases(MakeRange("chrM", 0, 100)),
              IsNotOKWithCodeAndMessage(
                  tensorflow::error::FAILED_PRECONDITION,
                  "can't read from closed IndexedFastaReader object"));
}

TEST(IndexedFastaReaderTest, TestIterate) {
  auto reader = JustLoadFai(TestFastaPath());
  auto iterator = reader->Iterate().ValueOrDie();
  GenomeReferenceRecord r;
  StatusOr<bool> status = iterator->Next(&r);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("chrM", r.first);
  EXPECT_EQ(
      "GATCACAGGTCTATCACCCTATTAACCACTCACGGGAGCTCTCCATGCATTTGGTATTTTCGTCTGGGGGGT"
      "GTGCACGCGATAGCATTGCGAGACGCTG",
      r.second);
  status = iterator->Next(&r);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("chr1", r.first);
  EXPECT_EQ(
      "ACCACCATCCTCCGTGAAATCAATATCCCGCACAAGAGTGCTACTCTCCTAAATCCCTTCTCGTCCCCATGG"
      "ATGA",
      r.second);
  status = iterator->Next(&r);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("chr2", r.first);
  EXPECT_EQ(
      "CGCTNCGGGCCCATAACACTTGGGGGTAGCTAAAGTGAACTGTATCCGACATCTGGTTCCTACTTCAGGGCC"
      "ATAAAGCCTAAATAGCCCACACGTTCCCCTTAAATAAGACATCACGATG",
      r.second);

  // Reading beyond the file fails.
  status = iterator->Next(&r);
  EXPECT_FALSE(status.ValueOrDie());
}

}  // namespace nucleus
