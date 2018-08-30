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

#include "htslib/hts.h"

#include <errno.h>
#include <stdlib.h>
#include <string>

#include "tensorflow/core/platform/test.h"
#include "absl/strings/str_cat.h"
#include "htslib/hts_log.h"
#include "htslib/kstring.h"
#include "nucleus/io/hts_path.h"
#include "nucleus/platform/types.h"
#include "nucleus/testing/test_utils.h"
#include "nucleus/util/port.h"

using absl::StrCat;

namespace nucleus {

// This tests minimal htslib functionality.
TEST(FileTest, HtsOpen) {
  hts_set_log_level(HTS_LOG_TRACE);

  string path = GetTestData("test.fasta");
  errno = 0;
  htsFile* f = hts_open(path.c_str(), "rb");
  EXPECT_EQ(errno, 0);
  ASSERT_NE(f, nullptr);

  kstring_t str = {0, 0, nullptr};
  int n = hts_getline(f, '\n', &str);
  EXPECT_GT(n, 0);
  EXPECT_EQ(ks_len(&str), 5);
  EXPECT_EQ(ks_str(&str), string(">chrM"));
  free(str.s);

  int close_ok = hts_close(f);
  EXPECT_EQ(close_ok, 0);
}

}  // namespace nucleus
