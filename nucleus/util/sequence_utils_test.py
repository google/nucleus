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

"""Tests for nucleus.util.sequence_utils."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from absl.testing import parameterized

from nucleus.util import sequence_utils


class SequenceUtilsTests(parameterized.TestCase):

  @parameterized.parameters(
      dict(seq='', expected=''),
      dict(seq='A', expected='T'),
      dict(seq='T', expected='A'),
      dict(seq='C', expected='G'),
      dict(seq='G', expected='C'),
      dict(seq='GGGCAGATT', expected='AATCTGCCC'),
      dict(
          seq='GGGCAGANN',
          expected='NNTCTGCCC',
          complement_dict=sequence_utils.DNA_COMPLEMENT_UPPER),
      dict(
          seq='accgt',
          expected='acggt',
          complement_dict=sequence_utils.DNA_COMPLEMENT),
      dict(
          seq='ATCGRYSWKMBVDHN',
          expected='NDHBVKMWSRYCGAT',
          complement_dict=sequence_utils.IUPAC_DNA_COMPLEMENT_UPPER),
      dict(
          seq='ATCGRYSWKMBVDHNatcgryswkmbvdhn',
          expected='ndhbvkmwsrycgatNDHBVKMWSRYCGAT',
          complement_dict=sequence_utils.IUPAC_DNA_COMPLEMENT),
  )
  def test_reverse_complement(self, seq, expected, complement_dict=None):
    """Tests canonical DNA sequences are reverse complemented correctly."""
    self.assertEqual(
        sequence_utils.reverse_complement(seq, complement_dict), expected)

  @parameterized.parameters(
      dict(seq='GGGCAGANN'),
      dict(seq='accgt'),
      dict(
          seq='ATCGRYSWKMBVDHNatcgryswkmbvdhn',
          complement_dict=sequence_utils.IUPAC_DNA_COMPLEMENT_UPPER),
      dict(seq='X', complement_dict=sequence_utils.IUPAC_DNA_COMPLEMENT),
  )
  def test_bad_reverse_complement(self, seq, complement_dict=None):
    """Tests error is raised when complement_dict does not cover given seq."""
    with self.assertRaisesRegexp(sequence_utils.Error, 'Unknown base in'):
      sequence_utils.reverse_complement(seq, complement_dict)

  @parameterized.parameters(
      dict(
          bases_set=sequence_utils.STRICT_DNA_BASES_UPPER,
          complement_dict=sequence_utils.STRICT_DNA_COMPLEMENT_UPPER),
      dict(
          bases_set=sequence_utils.STRICT_DNA_BASES,
          complement_dict=sequence_utils.STRICT_DNA_COMPLEMENT),
      dict(
          bases_set=sequence_utils.DNA_BASES_UPPER,
          complement_dict=sequence_utils.DNA_COMPLEMENT_UPPER),
      dict(
          bases_set=sequence_utils.DNA_BASES,
          complement_dict=sequence_utils.DNA_COMPLEMENT),
  )
  def test_base_set_definitions(self, bases_set, complement_dict):
    """Tests that base set and complement dict definitions are consistent."""
    self.assertEqual(bases_set, frozenset(complement_dict.keys()))


if __name__ == '__main__':
  absltest.main()
