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

"""This example program adds the AD info field to a VCF file.

It assumes that the AD field of the individual variant calls is already
populated.

Sample usage:
  $ add_ad_to_vcf input.vcf.gz output.vcf.gz
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys

from absl import app
import six
from nucleus.io import vcf
from nucleus.util import variant_utils
from nucleus.util import variantcall_utils
from nucleus.util import vcf_constants


def get_variant_ad(variant):
  """Returns the allele depth for the Variant, calculated across its calls."""
  num_alleles = len(variant.alternate_bases) + 1
  call_ads = [variantcall_utils.get_format(vc, 'AD') for vc in
              variant.calls]
  assert(len(call_ad) == num_alleles for call_ad in call_ads)
  return [sum(call_ad[i] for call_ad in call_ads)
          for i in six.moves.xrange(num_alleles)]


def main(argv):
  if len(argv) != 3:
    print('Usage: %s <input_vcf> <output_vcf>' % argv[0])
    sys.exit(-1)
  in_vcf = argv[1]
  out_vcf = argv[2]

  with vcf.VcfReader(in_vcf) as reader:
    if 'AD' in [info.id for info in reader.header.infos]:
      print('%s already contains AD field.' % in_vcf)
      sys.exit(-1)
    out_header = reader.header
    out_header.infos.extend([vcf_constants.reserved_info_field('AD')])

    with vcf.VcfWriter(out_vcf, header=out_header) as writer:
      for variant in reader:
        variant_utils.set_info(variant, 'AD', get_variant_ad(variant),
                               writer)
        writer.write(variant)


if __name__ == '__main__':
  app.run(main)
