# Copyright 2019 Google LLC.
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
"""Tests for nucleus.io.tabix."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import shutil

from absl.testing import absltest

from nucleus.io import tabix
from nucleus.io import vcf
from nucleus.testing import test_utils
from nucleus.util import ranges
from tensorflow.python.platform import gfile


class TabixTest(absltest.TestCase):
  """Test the functionality of tabix.build_index."""

  def setUp(self):
    super(TabixTest, self).setUp()
    self.input_file = test_utils.genomics_core_testdata('test_samples.vcf.gz')
    self.output_file = test_utils.test_tmpfile('test_samples.vcf.gz')
    shutil.copyfile(self.input_file, self.output_file)
    self.tbx_index_file = self.output_file + '.tbi'

  def tearDown(self):
    super(TabixTest, self).tearDown()
    os.remove(self.output_file)
    try:
      os.remove(self.tbx_index_file)
    except OSError:
      pass

  def test_build_index(self):
    self.assertFalse(gfile.Exists(self.tbx_index_file))
    tabix.build_index(self.output_file)
    self.assertTrue(gfile.Exists(self.tbx_index_file))

  def test_vcf_query(self):
    tabix.build_index(self.output_file)
    self.input_reader = vcf.VcfReader(self.input_file)
    self.output_reader = vcf.VcfReader(self.output_file)

    range1 = ranges.parse_literal('chr3:100,000-500,000')
    self.assertEqual(
        list(self.input_reader.query(range1)),
        list(self.output_reader.query(range1)))


if __name__ == '__main__':
  absltest.main()
