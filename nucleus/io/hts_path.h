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

#ifndef THIRD_PARTY_NUCLEUS_IO_HTS_PATH_H_
#define THIRD_PARTY_NUCLEUS_IO_HTS_PATH_H_

#include <string>

#include "htslib/faidx.h"
#include "htslib/hts.h"
#include "htslib/tbx.h"

namespace nucleus {

// This is a wrapper for hts_open that lets us select a default
// protocol, like "file:" or just plain "".
htsFile *hts_open_x(const std::string &fn, const char *mode);

htsFile *hts_open_format_x(const std::string &fn, const char *mode,
                           htsFormat *fmt);

faidx_t *fai_load3_x(const std::string &fa_path, const std::string &fai_path,
                     const std::string &gzi_path, int flags);

int tbx_index_build_x(const std::string &fn, int min_shift,
                      const tbx_conf_t *conf);

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_HTS_PATH_H_
