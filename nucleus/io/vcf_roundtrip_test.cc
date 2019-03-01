/*
 * Copyright 2019 Google LLC.
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

#include <memory>
#include <vector>

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/io/vcf_reader.h"
#include "nucleus/io/vcf_writer.h"
#include "nucleus/platform/types.h"
#include "nucleus/protos/variants.pb.h"
#include "nucleus/testing/protocol-buffer-matchers.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/vendor/status_matchers.h"

namespace nucleus {

namespace {
using genomics::v1::Variant;
}  // namespace

// Read from a vcf file and write to a bcf file. The bcf file should have the
// same contents as the vcf file.
TEST(VcfRoundtripTest, ReadVCFWriteBCF) {
  string input_file = GetTestData("test_sites.vcf");
  string output_file = MakeTempFile("output.bcf.gz");
  auto reader = std::move(
      VcfReader::FromFile(input_file, genomics::v1::VcfReaderOptions())
          .ValueOrDie());
  auto writer = std::move(VcfWriter::ToFile(output_file, reader->Header(),
                                            genomics::v1::VcfWriterOptions())
                              .ValueOrDie());
  std::vector<Variant> expected_variants = as_vector(reader->Iterate());
  for (const auto& v : expected_variants) {
    ASSERT_THAT(writer->Write(v), IsOK());
  }
  writer = nullptr;
  auto output_reader = std::move(
      VcfReader::FromFile(output_file, genomics::v1::VcfReaderOptions())
          .ValueOrDie());
  EXPECT_THAT(as_vector(output_reader->Iterate()),
              testing::Pointwise(EqualsProto(), expected_variants));
}

}  // namespace nucleus
