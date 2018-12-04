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

"""Writes all the variants in a VCF file with a quality greater than 3.01."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys

from absl import app

from nucleus.io import vcf


def main(argv):
  if len(argv) != 3:
    print('Usage: {} <input_vcf> <output_vcf>'.format(argv[0]))
    sys.exit(-1)
  in_vcf = argv[1]
  out_vcf = argv[2]

  # Please try to keep the following part in sync with the documenation in
  # g3doc/overview.md.
  with vcf.VcfReader(in_vcf) as reader:
    print('Sample names in VCF: ', ' '.join(reader.header.sample_names))
    with vcf.VcfWriter(out_vcf, header=reader.header) as writer:
      for variant in reader:
        if variant.quality > 3.01:
          writer.write(variant)


if __name__ == '__main__':
  app.run(main)
