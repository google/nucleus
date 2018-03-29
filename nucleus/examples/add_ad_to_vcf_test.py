# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tests for nucleus.examples.add_ad_to_vcf."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function



from absl.testing import absltest

from nucleus.examples import add_ad_to_vcf
from nucleus.io import vcf
from nucleus.testing import test_utils
from nucleus.util import variant_utils


class AddAdToVcfTest(absltest.TestCase):

  def test_main(self):
    in_fname = test_utils.genomics_core_testdata('test_allele_depth.vcf')
    out_fname = test_utils.test_tmpfile('output.vcf')
    add_ad_to_vcf.main(['add_ad_to_vcf', in_fname, out_fname])

    with vcf.VcfReader(out_fname, use_index=False) as reader:
      info_ids = [info.id for info in reader.header.infos]
      self.assertTrue('AD' in info_ids)
      variant1 = next(reader)
      self.assertEqual([3, 3], variant_utils.get_info(variant1, 'AD', reader))
      variant2 = next(reader)
      self.assertEqual([30, 44], variant_utils.get_info(variant2, 'AD', reader))
      variant3 = next(reader)
      self.assertEqual([15, 4], variant_utils.get_info(variant3, 'AD', reader))
      variant4 = next(reader)
      self.assertEqual([2, 4], variant_utils.get_info(variant4, 'AD', reader))
      variant5 = next(reader)
      self.assertEqual([24, 2], variant_utils.get_info(variant5, 'AD', reader))


if __name__ == '__main__':
  absltest.main()
