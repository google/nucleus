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
"""Tests for nucleus.examples.apply_genotyping_prior."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import math

from absl.testing import absltest

from nucleus.examples import apply_genotyping_prior
from nucleus.protos import variants_pb2


class RecallVariantsTest(absltest.TestCase):

  def test_recall_variant(self):
    variant = variants_pb2.Variant(reference_bases='A',
                                   alternate_bases=['T'])
    call = variant.calls.add()
    call.genotype[:] = [0, 1]
    call.genotype_likelihood[:] = [-2.5, -1.0, -2.0]

    log_third = math.log10(1.0 / 3.0)
    flat_priors = [log_third, log_third, log_third]
    apply_genotyping_prior.recall_variant(flat_priors, variant)
    self.assertEqual([0, 1], variant.calls[0].genotype)

    realistic_priors = [math.log10(0.99), -2.0, -2.0]
    apply_genotyping_prior.recall_variant(realistic_priors, variant)
    self.assertEqual([0, 0], variant.calls[0].genotype)


if __name__ == '__main__':
  absltest.main()

