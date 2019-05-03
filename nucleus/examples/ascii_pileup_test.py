# Copyright 2019 Google LLC.
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
"""Tests for nucleus.examples.ascii_pileup."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.examples import ascii_pileup
from nucleus.testing import test_utils


class AsciiPileupTest(absltest.TestCase):

  def test_read_str(self):
    s = ascii_pileup.read_str(10, 15, 20, 'AACCGGTTAACCGGTT')
    self.assertTrue(s.startswith('     AACCG'))
    self.assertTrue(s.endswith('TTAACCGGTT'))

  def test_ascii_pileup(self):
    in_sam = test_utils.genomics_core_testdata('test.bam')

    self.assertEqual([], ascii_pileup.ascii_pileup(in_sam, 'chr1:10050'))
    ap = ascii_pileup.ascii_pileup(in_sam, 'chr20:9999999')
    self.assertLen(ap, 45)
    self.assertEqual(
        'CAACTGACCATAGGTGTATTGGTTTATTTCTGTACTCTTAGTAGATTCCATTGACCTATATCTCT'
        'ATCCTTATGCCAGTACCACACT' + ascii_pileup.ANSI_BOLD +
        ascii_pileup.ANSI_RED + 'G' + ascii_pileup.ANSI_OFF + 'TTTTGTTTACTAC',
        ap[0])


if __name__ == '__main__':
  absltest.main()
