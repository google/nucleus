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

#include "nucleus/io/fasta_reader.h"

#include <string>

#include "nucleus/util/utils.h"

#include "tensorflow/core/platform/test.h"

namespace nucleus {

namespace {

// Helper method to create a test sequence.
void CreateTestSeq(std::vector<genomics::v1::ContigInfo>* contigs,
                   std::vector<genomics::v1::ReferenceSequence>* seqs,
                   const string& name, const int pos_in_fasta,
                   const int range_start, const int range_end,
                   const string& bases) {
  DCHECK(pos_in_fasta >= 0 && pos_in_fasta < contigs->size());
  genomics::v1::ContigInfo* contig = &contigs->at(pos_in_fasta);
  contig->set_name(name);
  contig->set_pos_in_fasta(pos_in_fasta);
  contig->set_n_bases(range_end - range_start);
  genomics::v1::ReferenceSequence* seq = &seqs->at(pos_in_fasta);
  seq->mutable_region()->set_reference_name(name);
  seq->mutable_region()->set_start(range_start);
  seq->mutable_region()->set_end(range_end);
  seq->set_bases(bases);
}

}  // namespace

TEST(FastaReaderTest, TestIterate) {
  int kNum = 3;
  std::vector<genomics::v1::ContigInfo> contigs(kNum);
  std::vector<genomics::v1::ReferenceSequence> seqs(kNum);
  CreateTestSeq(&contigs, &seqs, "Chr1", 0, 0, 1, "A");
  CreateTestSeq(&contigs, &seqs, "Chr2", 1, 4, 6, "CG");
  CreateTestSeq(&contigs, &seqs, "Chr3", 2, 10, 15, "AATTC");

  std::unique_ptr<InMemoryGenomeReference> reader =
      std::move(InMemoryGenomeReference::Create(contigs, seqs).ValueOrDie());
  auto iterator = reader->Iterate().ValueOrDie();
  GenomeReferenceRecord r;
  StatusOr<bool> status = iterator->Next(&r);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("Chr1", r.first);
  EXPECT_EQ("A", r.second);
  status = iterator->Next(&r);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("Chr2", r.first);
  EXPECT_EQ("CG", r.second);
  status = iterator->Next(&r);
  EXPECT_TRUE(status.ValueOrDie());
  EXPECT_EQ("Chr3", r.first);
  EXPECT_EQ("AATTC", r.second);

  // Reading beyond the file fails.
  status = iterator->Next(&r);
  EXPECT_FALSE(status.ValueOrDie());
}

}  // namespace nucleus
