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
#include <string>

#include "nucleus/io/tfrecord_reader.h"

#include "nucleus/protos/variants.pb.h"
#include "nucleus/testing/test_utils.h"

namespace nucleus {

TEST(TFRecordReaderTest, Simple) {
  std::unique_ptr<TFRecordReader> reader = TFRecordReader::New(
      GetTestData("test_likelihoods.vcf.golden.tfrecord"), "");
  ASSERT_NE(reader, nullptr);

  ASSERT_TRUE(reader->GetNext());

  std::string s = reader->record();

  nucleus::genomics::v1::Variant v;
  v.ParseFromString(s);

  ASSERT_EQ("Chr1", v.reference_name());

  reader->Close();
}


TEST(TFRecordReaderTest, NotFound) {
  std::unique_ptr<TFRecordReader> reader =
      TFRecordReader::New(GetTestData("not_found.tfrecord"), "");
  ASSERT_EQ(reader, nullptr);
}

}  // namespace nucleus

