#!/bin/bash
# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This script extracts ```bash ``` blocks from markdown files.  We
# can use this as a minimal form of literate programming, so that the
# instructions in our tutorial documents are more easily exercised.
#
# usage: < foo.md > bar.sh
#
echo '#!/bin/bash'
echo 'set -e'
echo 'set -u'
echo 'set -o pipefail'
echo ''
perl -ne '
  print "\n### line $.\n" if /^```bash/ ;
  print if /^```bash/ ... /^```/ and !/^```/
' "$@"
echo ''
echo 'echo status=$?'
