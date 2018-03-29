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
    self.assertEquals(
        ranges.make_range('chrX', start, start + 5), utils.read_range(read))
    read = test_utils.make_read(
        'AAACAG',
        chrom='chrX',
        start=start,
        cigar='2M16D3M',
        quals=range(10, 16),
        name='read1')
    self.assertEquals(
        ranges.make_range('chrX', start, start + 5 + 16),
        utils.read_range(read))

  def test_reservoir_sample_length(self):
    """Tests samples have expected length."""
    first_ten_ints = range(10)
    # Test sampling with k > len(iterable).
    self.assertEquals(len(utils.reservoir_sample(first_ten_ints, 11)), 10)
    # Test sampling with k == len(iterable).
    self.assertEquals(len(utils.reservoir_sample(first_ten_ints, 10)), 10)
    # Test sampling with k < len(iterable).
    self.assertEquals(len(utils.reservoir_sample(first_ten_ints, 9)), 9)
    # Test sampling with k == 0.
    self.assertEquals(len(utils.reservoir_sample(first_ten_ints, 0)), 0)
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
    for c in counts.itervalues():
      observed_frequency = c / float(n_replicates)
      npt.assert_allclose(observed_frequency, expected_frequency, atol=0.01)


if __name__ == '__main__':
  absltest.main()
