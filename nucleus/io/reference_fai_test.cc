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

#include "nucleus/io/reference_fai.h"

#include <memory>
#include <utility>
#include <vector>

#include "nucleus/io/reference_test.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"

#include "tensorflow/core/lib/strings/strcat.h"
#include "tensorflow/core/platform/logging.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>


#include "tensorflow/core/platform/test.h"

using std::make_pair;
using tensorflow::strings::StrCat;
using testing::Eq;
using testing::StartsWith;

namespace nucleus {

static std::unique_ptr<GenomeReference>
JustLoadFai(const string& fasta, int cache_size = 64 * 1024) {
  StatusOr<std::unique_ptr<GenomeReferenceFai>> fai_status =
      GenomeReferenceFai::FromFile(fasta, StrCat(fasta, ".fai"), cache_size);
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
  StatusOr<std::unique_ptr<GenomeReferenceFai>> result =
      GenomeReferenceFai::FromFile(GetTestData("unindexed.fasta"),
                                   GetTestData("unindexed.fasta.fai"));
  EXPECT_THAT(result, IsNotOKWithCodeAndMessage(
      tensorflow::error::NOT_FOUND,
      "could not load fasta and/or fai for fasta"));
}

TEST(ReferenceFaiTest, WriteAfterCloseIsntOK) {
  auto reader = JustLoadFai(TestFastaPath());
  ASSERT_THAT(reader->Close(), IsOK());
  EXPECT_THAT(reader->GetBases(MakeRange("chrM", 0, 100)),
              IsNotOKWithCodeAndMessage(
                  tensorflow::error::FAILED_PRECONDITION,
                  "can't read from closed GenomeReferenceFai object"));
}

}  // namespace nucleus
