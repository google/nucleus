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
"""Tests for nucleus.examples.count_variants."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
import mock
import six

from nucleus.examples import count_variants
from nucleus.testing import test_utils


class CountVariantsTest(absltest.TestCase):

  def test_main(self):
    in_fname = test_utils.genomics_core_testdata('test_allele_depth.vcf')
    with mock.patch.object(six.moves.builtins, 'print') as mock_print:
      count_variants.main(['count_variants', in_fname])
      self.assertEqual([
          mock.call('# variants: 5'),
          mock.call('# ref variants: 0'),
          mock.call('# SNP variants: 5'),
          mock.call('# indel variants: 0'),
          mock.call('# variants in Chr1: 5'),
      ], mock_print.mock_calls)


if __name__ == '__main__':
  absltest.main()
