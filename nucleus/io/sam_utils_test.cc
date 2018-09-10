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

#include "nucleus/io/sam_utils.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "htslib/sam.h"
#include "nucleus/protos/cigar.pb.h"

namespace nucleus {

using genomics::v1::CigarUnit;
using genomics::v1::CigarUnit_Operation_Operation_MAX;
using genomics::v1::CigarUnit_Operation_Operation_MIN;

TEST(SamUtilsTest, Conversion) {
  for (int i = CigarUnit_Operation_Operation_MIN;
       i <= CigarUnit_Operation_Operation_MAX; ++i) {
    CigarUnit::Operation op = static_cast<CigarUnit::Operation>(i);
    EXPECT_EQ(op, kHtslibCigarToProto[kProtoToHtslibCigar[i]]);
  }
}

TEST(SamUtilsTest, ProtoConversion) {
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::ALIGNMENT_MATCH], BAM_CMATCH);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::INSERT], BAM_CINS);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::DELETE], BAM_CDEL);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::SKIP], BAM_CREF_SKIP);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::CLIP_SOFT], BAM_CSOFT_CLIP);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::CLIP_HARD], BAM_CHARD_CLIP);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::PAD], BAM_CPAD);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::SEQUENCE_MATCH], BAM_CEQUAL);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::SEQUENCE_MISMATCH], BAM_CDIFF);
  EXPECT_EQ(kProtoToHtslibCigar[CigarUnit::OPERATION_UNSPECIFIED], BAM_CBACK);
}

}  // namespace nucleus
