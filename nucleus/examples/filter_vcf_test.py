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
"""Tests for nucleus.examples.filter_vcf."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.examples import filter_vcf
from nucleus.io import vcf
from nucleus.testing import test_utils


class FilterVcfTest(absltest.TestCase):

  def test_main(self):
    in_fname = test_utils.genomics_core_testdata('test_vaf.vcf')
    out_fname = test_utils.test_tmpfile('output.vcf')
    filter_vcf.main(['filter_vcf', in_fname, out_fname])

    with vcf.VcfReader(out_fname) as reader:
      variants = list(reader)
      self.assertEqual(3, len(variants))
      self.assertEqual(['DogSNP4', 'DogSNP5', 'DogSNP6'],
                       [v.names[0] for v in variants])
      for v in variants:
        self.assertTrue(v.quality > 3.01)


if __name__ == '__main__':
  absltest.main()
