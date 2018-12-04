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
"""Counts variants in a VCF, both by type and per chromosome."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import collections
import sys

from absl import app
from nucleus.io import vcf
from nucleus.util import variant_utils


def main(argv):
  if len(argv) != 2:
    print('Usage: {} <input_vcf>'.format(argv[0]))
    sys.exit(-1)
  in_vcf = argv[1]

  total = 0
  by_type = collections.defaultdict(int)
  by_ref = collections.defaultdict(int)

  with vcf.VcfReader(in_vcf) as reader:
    for variant in reader:
      total += 1
      by_type[variant_utils.variant_type(variant)] += 1
      by_ref[variant.reference_name] += 1

  print('# variants: {}'.format(total))
  print('# ref variants: {}'.format(by_type[variant_utils.VariantType.ref]))
  print('# SNP variants: {}'.format(by_type[variant_utils.VariantType.snp]))
  print('# indel variants: {}'.format(by_type[variant_utils.VariantType.indel]))
  for k, v in sorted(by_ref.items()):
    print('# variants in {}: {}'.format(k, v))


if __name__ == '__main__':
  app.run(main)
