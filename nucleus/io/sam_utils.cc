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

#include "nucleus/io/sam_utils.h"

#include "htslib/sam.h"

namespace nucleus {

const char kSamReadGroupTag[] = "@RG";
const char kSamProgramTag[] = "@PG";
const char kSamCommentTag[] = "@CO";
const char kSamHeaderTag[] = "@HD";
const char kSamReferenceSequenceTag[] = "@SQ";

const char kIDTag[] = "ID:";
const char kCNTag[] = "CN:";
const char kDSTag[] = "DS:";
const char kDTTag[] = "DT:";
const char kFOTag[] = "FO:";
const char kKSTag[] = "KS:";
const char kLBTag[] = "LB:";
const char kPGTag[] = "PG:";
const char kPITag[] = "PI:";
const char kPLTag[] = "PL:";
const char kPMTag[] = "PM:";
const char kPUTag[] = "PU:";
const char kSMTag[] = "SM:";
const char kPNTag[] = "PN:";
const char kCLTag[] = "CL:";
const char kPPTag[] = "PP:";
const char kVNTag[] = "VN:";
const char kSOTag[] = "SO:";
const char kGOTag[] = "GO:";

const int kProtoToHtslibCigar[] = {
    // genomics::v1::CigarUnit::OPERATION_UNSPECIFIED,
    BAM_CBACK,
    // genomics::v1::CigarUnit::ALIGNMENT_MATCH,
    BAM_CMATCH,
    // genomics::v1::CigarUnit::INSERT,
    BAM_CINS,
    // genomics::v1::CigarUnit::DELETE,
    BAM_CDEL,
    // genomics::v1::CigarUnit::SKIP,
    BAM_CREF_SKIP,
    // genomics::v1::CigarUnit::CLIP_SOFT,
    BAM_CSOFT_CLIP,
    // genomics::v1::CigarUnit::CLIP_HARD,
    BAM_CHARD_CLIP,
    // genomics::v1::CigarUnit::CPAD,
    BAM_CPAD,
    // genomics::v1::CigarUnit::SEQUENCE_MATCH,
    BAM_CEQUAL,
    // genomics::v1::CigarUnit::SEQUENCE_MISMATCH,
    BAM_CDIFF,
};

// Array mapping htslib BAM constants (in comment) to proto
// genomics::v1::CigarUnit enum values.
const genomics::v1::CigarUnit_Operation kHtslibCigarToProto[] = {
    // #define BAM_CMATCH      0
    genomics::v1::CigarUnit::ALIGNMENT_MATCH,
    // #define BAM_CINS        1
    genomics::v1::CigarUnit::INSERT,
    // #define BAM_CDEL        2
    genomics::v1::CigarUnit::DELETE,
    // #define BAM_CREF_SKIP   3
    genomics::v1::CigarUnit::SKIP,
    // #define BAM_CSOFT_CLIP  4
    genomics::v1::CigarUnit::CLIP_SOFT,
    // #define BAM_CHARD_CLIP  5
    genomics::v1::CigarUnit::CLIP_HARD,
    // #define BAM_CPAD        6
    genomics::v1::CigarUnit::PAD,
    // #define BAM_CEQUAL      7
    genomics::v1::CigarUnit::SEQUENCE_MATCH,
    // #define BAM_CDIFF       8
    genomics::v1::CigarUnit::SEQUENCE_MISMATCH,
    // #define BAM_CBACK       9
    genomics::v1::CigarUnit::OPERATION_UNSPECIFIED,
};

}  // namespace nucleus
