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

#include "nucleus/io/hts_path.h"

#include <stddef.h>

#include "absl/strings/str_cat.h"
#include "nucleus/platform/types.h"

using absl::StrCat;

namespace nucleus {

const char dflt[] = "";

// Use the default file scheme, unless one is provided.
string fix_path(const std::string &path) {
  size_t i = path.find(':');
  if (i > 0 && i < 20) {
    return path;
  }
  return StrCat(dflt, path);
}

htsFile *hts_open_x(const std::string &fn, const char *mode) {
  string new_path = fix_path(fn);
  return hts_open(new_path.c_str(), mode);
}

htsFile *hts_open_format_x(const std::string &fn, const char *mode,
                           htsFormat *fmt) {
  string new_path = fix_path(fn);
  return hts_open_format(new_path.c_str(), mode, fmt);
}

faidx_t *fai_load3_x(const std::string &fa, const std::string &fai,
                     const std::string &gzi, int flags) {
  string nfa = fix_path(fa);
  string nfai = fix_path(fai);
  string ngzi = fix_path(gzi);
  return fai_load3(nfa.c_str(), nfai.c_str(), ngzi.c_str(), flags);
}

int tbx_index_build_x(const std::string &fn, int min_shift,
                      const tbx_conf_t *conf) {
  string new_path = fix_path(fn);
  return tbx_index_build(new_path.c_str(), min_shift, conf);
}

}  // namespace nucleus
