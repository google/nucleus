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

#include "nucleus/io/reference.h"

#include <vector>

#include "nucleus/io/reference_test.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"

#include "tensorflow/core/platform/logging.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"

namespace nucleus {

using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::UnorderedElementsAre;

TEST_P(GenomeReferenceTest, TestBasic) {
  EXPECT_THAT(Ref().ContigNames(),
              UnorderedElementsAre("chrM", "chr1", "chr2"));
  EXPECT_THAT(Ref().Contigs().size(), 3);

  const auto& chrm = *Ref().Contig("chrM").ValueOrDie();
  EXPECT_EQ(100, chrm.n_bases());
  EXPECT_EQ("chrM", chrm.name());
  EXPECT_EQ(0, chrm.pos_in_fasta());

  const auto& chr1 = *Ref().Contig("chr1").ValueOrDie();
  EXPECT_EQ(76, chr1.n_bases());
  EXPECT_EQ("chr1", chr1.name());
  EXPECT_EQ(1, chr1.pos_in_fasta());

  const auto& chr2 = *Ref().Contig("chr2").ValueOrDie();
  EXPECT_EQ(121, chr2.n_bases());
  EXPECT_EQ("chr2", chr2.name());
  EXPECT_EQ(2, chr2.pos_in_fasta());
}


TEST_P(GenomeReferenceTest, TestIsValidInterval) {
  // Checks that we can check that an unknown chromosome isn't valid.
  EXPECT_FALSE(Ref().IsValidInterval(MakeRange("unknown_chr", 0, 1)));

  for (const auto& chr : Ref().ContigNames()) {
    const auto n_bases = Ref().Contig(chr).ValueOrDie()->n_bases();

    EXPECT_TRUE(Ref().IsValidInterval(MakeRange(chr, 0, n_bases)));
    for (int i = 0; i < n_bases; ++i) {
      EXPECT_TRUE(Ref().IsValidInterval(MakeRange(chr, 0, i+1)));
      EXPECT_TRUE(Ref().IsValidInterval(MakeRange(chr, i, i+1)));
    }
    EXPECT_FALSE(Ref().IsValidInterval(MakeRange(chr, -10, 0)));
    EXPECT_FALSE(Ref().IsValidInterval(MakeRange(chr, -1, 0)));
    EXPECT_FALSE(Ref().IsValidInterval(MakeRange(chr, 10, 9)));
    EXPECT_FALSE(Ref().IsValidInterval(MakeRange(chr, 0, n_bases + 1)));
    EXPECT_FALSE(Ref().IsValidInterval(MakeRange(chr, 0, n_bases + 100)));
    EXPECT_FALSE(Ref().IsValidInterval(MakeRange(chr, n_bases, n_bases)));
    EXPECT_FALSE(
        Ref().IsValidInterval(MakeRange(chr, n_bases + 100, n_bases + 100)));
  }
}

TEST_P(GenomeReferenceTest, NotOKIfContigCalledWithBadName) {
  EXPECT_THAT(Ref().Contig("missing"),
              IsNotOKWithMessage("Unknown contig missing"));
}

TEST_P(GenomeReferenceTest, NotOKIfIntervalIsInvalid) {
  // Asking for bad chromosome values produces death.
  StatusOr<string> result = Ref().GetBases(MakeRange("missing", 0, 1));
  EXPECT_THAT(result, IsNotOKWithCodeAndMessage(
      tensorflow::error::INVALID_ARGUMENT,
      "Invalid interval"));

  // Starting before 0 is detected.
  EXPECT_THAT(Ref().GetBases(MakeRange("chrM", -1, 1)),
              IsNotOKWithMessage("Invalid interval"));

  // chr1 exists, but this range's start is beyond the chr.
  EXPECT_THAT(Ref().GetBases(MakeRange("chr1", 1000, 1010)),
              IsNotOKWithMessage("Invalid interval"));

  // chr1 exists, but this range's end is beyond the chr.
  EXPECT_THAT(Ref().GetBases(MakeRange("chr1", 0, 1010)),
              IsNotOKWithMessage("Invalid interval"));
}

TEST_P(GenomeReferenceTest, TestHasContig) {
  EXPECT_TRUE(Ref().HasContig("chrM"));
  EXPECT_TRUE(Ref().HasContig("chr1"));
  EXPECT_TRUE(Ref().HasContig("chr2"));
  EXPECT_FALSE(Ref().HasContig("chr3"));
  EXPECT_FALSE(Ref().HasContig("chr"));
  EXPECT_FALSE(Ref().HasContig(""));
}

// Checks that GetBases work in all its forms for the given arguments.
void CheckGetBases(const GenomeReference& ref,
                   const string& chrom, const int64 start, const int64 end,
                   const string& expected_bases) {
  StatusOr<string> query = ref.GetBases(MakeRange(chrom, start, end));
  ASSERT_THAT(query, IsOK());
  EXPECT_THAT(query.ValueOrDie(), expected_bases);
}


TEST_P(GenomeReferenceTest, TestReferenceBases) {
  CheckGetBases(Ref(), "chrM", 0, 100,
                "GATCACAGGTCTATCACCCTATTAACCACTCACGGGAGCTCTCCATGCATTTGGTATTTTC"
                "GTCTGGGGGGTGTGCACGCGATAGCATTGCGAGACGCTG");

  CheckGetBases(Ref(), "chr1", 0, 76,
                "ACCACCATCCTCCGTGAAATCAATATCCCGCACAAGAGTGCTACTCTCCTAAATCCCTTCT"
                "CGTCCCCATGGATGA");

  CheckGetBases(Ref(), "chr2", 0, 121,
                "CGCTNCGGGCCCATAACACTTGGGGGTAGCTAAAGTGAACTGTATCCGAC"
                "ATCTGGTTCCTACTTCAGGGCCATAAAGCCTAAATAGCCCACACGTTCCC"
                "CTTAAATAAGACATCACGATG");
}


TEST_P(GenomeReferenceTest, TestGetBasesParts) {
  CheckGetBases(Ref(), "chrM", 0, 10, "GATCACAGGT");
  CheckGetBases(Ref(), "chrM", 0, 9, "GATCACAGG");
  CheckGetBases(Ref(), "chrM", 1, 9, "ATCACAGG");
  CheckGetBases(Ref(), "chrM", 3, 7, "CACA");
  CheckGetBases(Ref(), "chrM", 90, 100, "CGAGACGCTG");
  CheckGetBases(Ref(), "chrM", 90, 99, "CGAGACGCT");
  CheckGetBases(Ref(), "chrM", 91, 100, "GAGACGCTG");
  CheckGetBases(Ref(), "chrM", 92, 100, "AGACGCTG");
  CheckGetBases(Ref(), "chrM", 92, 99, "AGACGCT");
  CheckGetBases(Ref(), "chrM", 92, 98, "AGACGC");

  CheckGetBases(Ref(), "chrM", 0, 1, "G");
  CheckGetBases(Ref(), "chrM", 1, 2, "A");
  CheckGetBases(Ref(), "chrM", 2, 3, "T");
  CheckGetBases(Ref(), "chrM", 3, 4, "C");
  CheckGetBases(Ref(), "chrM", 4, 5, "A");
  CheckGetBases(Ref(), "chrM", 5, 6, "C");

  // crosses the boundary of the index when max_bin_size is 5
  CheckGetBases(Ref(), "chrM", 4, 6, "AC");

  // 0-bp interval requests should return the empty string.
  CheckGetBases(Ref(), "chrM", 0, 0, "");
  CheckGetBases(Ref(), "chrM", 10, 10, "");
}

}  // namespace nucleus
