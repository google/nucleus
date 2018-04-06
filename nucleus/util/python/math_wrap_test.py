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
"""Tests for Math CLIF python wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.util.python import math


class MathWrapTest(absltest.TestCase):

  def test_one_minus_log10_prob_to_phred(self):
    self.assertAlmostEqual(16.4277471723837,
                           math.log10_ptrue_to_phred(-0.01, 1000))

  def test_phred_to_prob(self):
    self.assertEqual(0.1, math.phred_to_perror(10))

  def test_phred_to_log10_prob(self):
    self.assertEqual(-1, math.phred_to_log10_perror(10))

  def test_prob_to_phred(self):
    self.assertEqual(10.0, math.perror_to_phred(0.1))

  def test_prob_to_rounded_phred(self):
    self.assertEqual(10, math.perror_to_rounded_phred(0.1))

  def test_prob_to_log10_prob(self):
    self.assertEqual(-1, math.perror_to_log10_perror(0.1))

  def test_log10_prob_to_phred(self):
    self.assertEqual(10, math.log10_perror_to_phred(-1))

  def test_log10_prob_to_rounded_phred(self):
    self.assertEqual(10, math.log10_perror_to_rounded_phred(-1))

  def test_log10_prob_to_prob(self):
    self.assertEqual(0.1, math.log10_perror_to_perror(-1))

  def test_zero_shift_log10_probs(self):
    self.assertSequenceEqual([0, -1, -2],
                             math.zero_shift_log10_probs([-1, -2, -3]))


if __name__ == '__main__':
  absltest.main()
