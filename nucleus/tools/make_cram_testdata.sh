#!/bin/bash
# Copyright 2018 Google LLC.
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
# This script processes our test_cram.sam file into CRAM formatted and indexed
# files used in our tests.
#
# usage: ./make_cram_testdata.sh from the nucleus root directory.
#
set -euo pipefail

for version in 3.0; do
for embed_ref in 0 1; do
  filename="test_cram.embed_ref_${embed_ref}_version_${version}.cram"
  samtools view -C \
    --output-fmt cram \
    --output-fmt-option version="${version}" \
    --output-fmt-option embed_ref="${embed_ref}" \
    third_party/nucleus/testdata/test_cram.sam \
    -T third_party/nucleus/testdata/test.fasta \
    -o "third_party/nucleus/testdata/${filename}"
  samtools index "third_party/nucleus/testdata/${filename}"
done
done
