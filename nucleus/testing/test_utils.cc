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

#include "nucleus/testing/test_utils.h"

#include "nucleus/protos/cigar.pb.h"
#include "nucleus/util/utils.h"
#include "tensorflow/core/lib/io/path.h"

namespace nucleus {

using nucleus::MakePosition;
using nucleus::genomics::v1::CigarUnit;
using nucleus::genomics::v1::CigarUnit_Operation;
using nucleus::genomics::v1::LinearAlignment;
using nucleus::genomics::v1::Read;

namespace {

// Gets the CigarUnit_Operation corresponding to the single character
// representation of that CIGAR option.  For example, 'M' returns
// CigarUnit::ALIGNMENT_MATCH.
CigarUnit_Operation parse_cigar_op_str(const char op) {
  switch (op) {
    case 'M':
      return CigarUnit::ALIGNMENT_MATCH;
    case '=':
      return CigarUnit::SEQUENCE_MATCH;
    case 'X':
      return CigarUnit::SEQUENCE_MISMATCH;
    case 'I':
      return CigarUnit::INSERT;
    case 'D':
      return CigarUnit::DELETE;
    case 'S':
      return CigarUnit::CLIP_SOFT;
    case 'P':
      return CigarUnit::PAD;
    case 'H':
      return CigarUnit::CLIP_HARD;
    case 'N':
      return CigarUnit::SKIP;
    default:
      LOG(FATAL) << "Unexpected cigar op " << op;
  }
}
}  // namespace

// Simple getter for test files in the right testdata path.
string GetTestData(StringPiece path, StringPiece test_data_dir) {
  const string test_srcdir = getenv("TEST_SRCDIR");
  const char* test_workspace = getenv("TEST_WORKSPACE");
  test_workspace = test_workspace ? test_workspace : kDefaultWorkspace;
  return tensorflow::io::JoinPath(test_srcdir, test_workspace, test_data_dir,
                                  path);
}

string MakeTempFile(StringPiece filename) {
  const string test_tmpdir = getenv("TEST_TMPDIR");
  return tensorflow::io::JoinPath(test_tmpdir, filename);
}

std::vector<nucleus::genomics::v1::ContigInfo> CreateContigInfos(
    const std::vector<string>& names, const std::vector<int>& positions) {
  std::vector<nucleus::genomics::v1::ContigInfo> contigs;
  QCHECK_EQ(names.size(), positions.size());
  nucleus::genomics::v1::ContigInfo contig;
  for (size_t i = 0; i < names.size(); i++) {
    contig.set_name(names[i]);
    contig.set_pos_in_fasta(positions[i]);
    contigs.push_back(contig);
  }
  return contigs;
}

Read MakeRead(const string& chr, const int start, const string& bases,
              const std::vector<string>& cigar_elements) {
  Read read;
  read.set_fragment_name("test read");
  read.set_aligned_sequence(bases);
  read.set_number_reads(2);
  read.set_proper_placement(true);

  for (size_t i = 0; i < bases.length(); ++i) {
    read.add_aligned_quality(30);
  }

  LinearAlignment& aln = *read.mutable_alignment();
  aln.set_mapping_quality(90);
  *aln.mutable_position() = MakePosition(chr, start);
  for (const auto& cigar_str : cigar_elements) {
    CigarUnit& cigar = *aln.add_cigar();
    cigar.set_operation(parse_cigar_op_str(cigar_str.back()));
    const string& cigar_len_str = cigar_str.substr(0, cigar_str.length() - 1);
    int value = atoi(cigar_len_str.c_str());  // NOLINT: safe here.
    cigar.set_operation_length(value);
  }

  return read;
}

}  // namespace nucleus
