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

"""Tests for nucleus.io.gfile."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.io import gfile
from nucleus.testing import test_utils


class GfileTest(absltest.TestCase):

  def test_exists(self):
    self.assertTrue(gfile.Exists(
        test_utils.genomics_core_testdata('test_regions.bedgraph')))
    self.assertFalse(gfile.Exists(
        test_utils.genomics_core_testdata('does_not_exist')))

  def test_glob(self):
    g1 = gfile.Glob(test_utils.genomics_core_testdata('test*'))
    self.assertGreater(len(g1), 1)
    self.assertIn(
        test_utils.genomics_core_testdata('test.bam'), g1)
    g2 = gfile.Glob(test_utils.genomics_core_testdata('does_not_exist*'))
    self.assertEqual([], g2)

  def test_reading(self):
    with gfile.Open(test_utils.genomics_core_testdata('headerless.sam')) as f:
      for line in f:
        self.assertTrue(line.startswith('SRR3656745'))

  def test_writing(self):
    path = test_utils.test_tmpfile('test_gfile')
    with gfile.Open(path, 'w') as f:
      f.write('test\n')
      f.write('end\n')

    with gfile.Open(path, 'r') as f2:
      lines = f2.readlines()

    self.assertEqual(['test\n', 'end\n'], lines)


if __name__ == '__main__':
  absltest.main()
