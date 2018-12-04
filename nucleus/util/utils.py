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
"""Utility functions for working with reads."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from nucleus.util import cigar
from nucleus.util import ranges


def read_range(read):
  """Creates a Range proto from the alignment of Read.

  Args:
    read: nucleus.genomics.v1.Read. The read to calculate the range for.

  Returns:
    A nucleus.genomics.v1.Range for read.
  """
  start = read.alignment.position.position
  end = start + cigar.alignment_length(read.alignment.cigar)
  return ranges.make_range(read.alignment.position.reference_name, start, end)


def reservoir_sample(iterable, k, random=None):
  """Samples k elements with uniform probability from an iterable.

  Selects a subset of k elements from n input elements with uniform probability
  without needing to hold all n elements in memory at the same time. This
  implementation has max space complexity O(min(k, n)), i.e., we allocate up to
  min(k, n) elements to store the samples. This means that we only use ~n
  elements when n is smaller than k, which can be important when k is large. If
  n elements are added to this sampler, and n <= k, all n elements will be
  retained. If n > k, each added element will be retained with a uniform
  probability of k / n.

  The order of the k retained samples from our n elements is undefined. In
  particular that means that the elements in the returned list can occur in a
  different order than they appeared in the iterable.

  More details about reservoir sampling (and the specific algorithm used here
  called Algorithm R) can be found on wikipedia:

  https://en.wikipedia.org/wiki/Reservoir_sampling#Algorithm_R

  Args:
    iterable: Python iterable. The iterable to sample from.
    k: int. The number of elements to sample.
    random: A random number generator or None.

  Returns:
    A list containing the k sampled elements.

  Raises:
    ValueError: If k is negative.
  """
  if k < 0:
    raise ValueError('k must be nonnegative, but got {}'.format(k))
  if random is None:
    random = np.random
  sample = []
  for i, item in enumerate(iterable):
    if len(sample) < k:
      sample.append(item)
    else:
      j = random.randint(0, i + 1)
      if j < k:
        sample[j] = item
  return sample
