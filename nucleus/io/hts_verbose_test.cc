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

#include "nucleus/io/hts_verbose.h"

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"

namespace {

using nucleus::HtsGetLogLevel;
using nucleus::HtsSetLogLevel;
using ::testing::Eq;

TEST(HtsVerboseTest, Set) {
  enum htsLogLevel level;

  HtsSetLogLevel(HTS_LOG_TRACE);
  level = HtsGetLogLevel();
  EXPECT_THAT(level, Eq(HTS_LOG_TRACE));

  HtsSetLogLevel(HTS_LOG_INFO);
  level = HtsGetLogLevel();
  EXPECT_THAT(level, Eq(HTS_LOG_INFO));
}

}  // namespace
