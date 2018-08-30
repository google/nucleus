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

#include "nucleus/io/hts_path.h"

#include <stddef.h>
#include <string>

#include "absl/strings/str_cat.h"
#include "htslib/faidx.h"
#include "htslib/hts.h"
#include "nucleus/platform/types.h"

using absl::StrCat;

namespace nucleus {

const char dflt[] = "";

// Use the default file scheme, unless one is provided.
string fix_path(const char *path) {
  string s(path);
  size_t i = s.find(':');
  if (i > 0 && i < 20) {
    return s;
  }
  return StrCat(dflt, s);
}

htsFile *hts_open_x(const char *path, const char *mode) {
  string new_path = fix_path(path);
  return hts_open(new_path.c_str(), mode);
}

faidx_t *fai_load3_x(const char *fa, const char *fai, const char *gzi,
                     int flags) {
  string nfa = fix_path(fa);
  string nfai = fix_path(fai);
  string ngzi = fix_path(gzi);
  return fai_load3(fa ? nfa.c_str() : nullptr, fai ? nfai.c_str() : nullptr,
                   gzi ? ngzi.c_str() : nullptr, flags);
}

}  // namespace nucleus
