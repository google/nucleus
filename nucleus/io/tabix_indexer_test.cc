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

#include "nucleus/io/tabix_indexer.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/io/vcf_reader.h"
#include "nucleus/io/vcf_writer.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/utils.h"
#include "nucleus/vendor/status_matchers.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

using ::testing::Test;

constexpr char kVcfIndexSamplesFilename[] = "test_samples.vcf.gz";

TEST(TabixIndexerTest, IndexBuildsCorrectly) {
  string output_filename = MakeTempFile("test_samples.vcf.gz");
  string output_tabix_index = output_filename + ".tbi";

  std::unique_ptr<nucleus::VcfReader> reader = std::move(
      nucleus::VcfReader::FromFile(GetTestData(kVcfIndexSamplesFilename),
                                   nucleus::genomics::v1::VcfReaderOptions())
          .ValueOrDie());

  nucleus::genomics::v1::VcfWriterOptions writer_options;
  std::unique_ptr<VcfWriter> writer =
      std::move(nucleus::VcfWriter::ToFile(output_filename, reader->Header(),
                                           writer_options)
                    .ValueOrDie());

  auto variants = nucleus::as_vector(reader->Iterate());
  for (const auto& v : variants) {
    TF_CHECK_OK(writer->Write(v));
  }

  EXPECT_THAT(TbxIndexBuild(output_filename), IsOK());
  EXPECT_THAT(reader->Query(MakeRange("chr3", 14318, 14319)), IsOK());
}

TEST(CSIIndexerTest, IndexBuildsCorrectly) {
  string output_filename = MakeTempFile("test_samples.vcf.gz");
  string output_csi_index = output_filename + ".csi";

  std::unique_ptr<nucleus::VcfReader> reader = std::move(
      nucleus::VcfReader::FromFile(GetTestData(kVcfIndexSamplesFilename),
                                   nucleus::genomics::v1::VcfReaderOptions())
          .ValueOrDie());

  nucleus::genomics::v1::VcfWriterOptions writer_options;
  std::unique_ptr<VcfWriter> writer =
      std::move(nucleus::VcfWriter::ToFile(output_filename, reader->Header(),
                                           writer_options)
                    .ValueOrDie());

  auto variants = nucleus::as_vector(reader->Iterate());
  for (const auto& v : variants) {
    TF_CHECK_OK(writer->Write(v));
  }

  EXPECT_THAT(CSIIndexBuild(output_filename, 14), IsOK());
  EXPECT_THAT(tensorflow::Env::Default()->FileExists(output_csi_index), IsOK());
  EXPECT_THAT(reader->Query(MakeRange("chr3", 14318, 14319)), IsOK());
}
}  // namespace nucleus
