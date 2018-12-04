/*
 * Copyright 2018 Google LLC.
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

#ifndef THIRD_PARTY_NUCLEUS_IO_REFERENCE_TEST_H_
#define THIRD_PARTY_NUCLEUS_IO_REFERENCE_TEST_H_

#include <memory>
#include <string>
#include <utility>

#include "tensorflow/core/platform/test.h"
#include "nucleus/io/reference.h"
#include "nucleus/platform/types.h"
#include "nucleus/testing/test_utils.h"

namespace nucleus {


string TestFastaPath() { return GetTestData("test.fasta"); }

typedef std::unique_ptr<GenomeReference> CreateGenomeReferenceFunc(
    const string& fasta_path, int cache_size);

// Tests are parameterized by: reader factory, cache size.
class GenomeReferenceTest : public ::testing::TestWithParam<
    std::pair<CreateGenomeReferenceFunc*, int>> {
 protected:
  void SetUp() override {
    ref_ = (*GetParam().first)(TestFastaPath(), GetParam().second);
  }
  const GenomeReference& Ref() const { return *ref_; }

  std::unique_ptr<const GenomeReference> ref_;
};

typedef GenomeReferenceTest GenomeReferenceDeathTest;

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_REFERENCE_TEST_H_
