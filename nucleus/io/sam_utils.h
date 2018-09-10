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

#ifndef THIRD_PARTY_NUCLEUS_IO_SAM_UTILS_H_
#define THIRD_PARTY_NUCLEUS_IO_SAM_UTILS_H_

#include "nucleus/platform/types.h"
#include "nucleus/protos/cigar.pb.h"

namespace nucleus {

// This lists the header record type codes defined in section 1.3 of
// https://samtools.github.io/hts-specs/SAMv1.pdf.
//
// Record type tags are found in the header section of SAM files. The
// two-character tag is preceded with @, and indicates the start of a header
// line.
extern const char kSamReadGroupTag[];
extern const char kSamProgramTag[];
extern const char kSamCommentTag[];
extern const char kSamHeaderTag[];
extern const char kSamReferenceSequenceTag[];

// This lists the data field tags defined in section 1.3 of
// https://samtools.github.io/hts-specs/SAMv1.pdf.
//
// Data field tags are found in header lines. Each field tag consists of two
// capital aphabet characters followed by a colon.
//
// These constants are named after k(two-letter-code)Tag, where the
// two-letter-code together with a colon is the contents of the tag strings.
extern const char kIDTag[];
extern const char kCNTag[];
extern const char kDSTag[];
extern const char kDTTag[];
extern const char kFOTag[];
extern const char kKSTag[];
extern const char kLBTag[];
extern const char kPGTag[];
extern const char kPITag[];
extern const char kPLTag[];
extern const char kPMTag[];
extern const char kPUTag[];
extern const char kSMTag[];
extern const char kPNTag[];
extern const char kCLTag[];
extern const char kPPTag[];
extern const char kVNTag[];
extern const char kSOTag[];
extern const char kGOTag[];

// Array mapping CigarUnit_Operation enum to htslib BAM constants.
extern const int kProtoToHtslibCigar[];

// Array mapping htslib BAM constants (in comment) to proto CigarUnit enum
// values.
extern const genomics::v1::CigarUnit_Operation kHtslibCigarToProto[];

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_SAM_UTILS_H_
