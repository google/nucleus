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

"""Tests for nucleus.util.utils."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import collections

from absl.testing import absltest

from absl.testing import parameterized
import numpy as np
import numpy.testing as npt

from nucleus.testing import test_utils
from nucleus.util import ranges
from nucleus.util import utils


class UtilsTest(parameterized.TestCase):

  def test_read_range(self):
    """Tests reads have their ranges calculated correctly."""
    start = 10000001
    read = test_utils.make_read(
        'AAACAG',
        chrom='chrX',
        start=start,
        cigar='2M1I3M',
        quals=range(10, 16),
        name='read1')
    self.assertEqual(
        ranges.make_range('chrX', start, start + 5), utils.read_range(read))
    read = test_utils.make_read(
        'AAACAG',
        chrom='chrX',
        start=start,
        cigar='2M16D3M',
        quals=range(10, 16),
        name='read1')
    self.assertEqual(
        ranges.make_range('chrX', start, start + 5 + 16),
        utils.read_range(read))

  def test_reservoir_sample_length(self):
    """Tests samples have expected length."""
    first_ten_ints = range(10)
    # Test sampling with k > len(iterable).
    self.assertEqual(len(utils.reservoir_sample(first_ten_ints, 11)), 10)
    # Test sampling with k == len(iterable).
    self.assertEqual(len(utils.reservoir_sample(first_ten_ints, 10)), 10)
    # Test sampling with k < len(iterable).
    self.assertEqual(len(utils.reservoir_sample(first_ten_ints, 9)), 9)
    # Test sampling with k == 0.
    self.assertEqual(len(utils.reservoir_sample(first_ten_ints, 0)), 0)
    # Test sampling with k < 0 (bad args).
    with self.assertRaises(ValueError):
      utils.reservoir_sample(first_ten_ints, -1)

  @parameterized.parameters(
      (10, 0),
      (1, 1),
      (10, 1),
      (1, 3),
      (3, 3),
      (6, 3),
      (10, 3),
  )
  def test_reservoir_sample_frequency(self, iterable_size, k):
    """Tests observed frequency is close to expected frequency."""
    # Use a fixed random number so our test is deterministic.
    random = np.random.RandomState(123456789)
    n_replicates = 100000
    counts = collections.Counter(
        item
        for _ in range(n_replicates)
        for item in utils.reservoir_sample(range(iterable_size), k, random))
    expected_frequency = min(k / float(iterable_size), 1.0)
    for c in counts.values():
      observed_frequency = c / float(n_replicates)
      npt.assert_allclose(observed_frequency, expected_frequency, atol=0.01)

  @parameterized.parameters(
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=4, e2=10, expected=False),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=3, e2=10, expected=False),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=2, e2=10, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=1, e2=10, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=0, e2=10, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=0, e2=1, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=0, e2=2, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=0, e2=3, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=1, e2=2, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=1, e2=3, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=2, e2=3, expected=True),
      # dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=3, e2=3, expected=False),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=0, e2=4, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr1', s2=1, e2=4, expected=True),
      dict(ref1='chr1', s1=0, e1=3, ref2='chr2', s2=1, e2=4, expected=False),
  )
  def test_read_overlaps_region(self, ref1, s1, e1, ref2, s2, e2, expected):

    def check_overlaps(chr1, start1, end1, chr2, start2, end2, expected):
      nbp = end1 - start1
      read = test_utils.make_read(
          'A' * nbp, chrom=chr1, start=start1, cigar='{}M'.format(nbp))
      region = ranges.make_range(chr2, start2, end2)
      self.assertEqual(utils.read_overlaps_region(read, region), expected)
      # This check ensures we get the same result calling ranges.ranges_overlap.
      self.assertEqual(
          ranges.ranges_overlap(region, utils.read_range(read)), expected)

    check_overlaps(ref1, s1, e1, ref2, s2, e2, expected)
    check_overlaps(ref2, s2, e2, ref1, s1, e1, expected)


if __name__ == '__main__':
  absltest.main()
