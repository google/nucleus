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
"""Tests for nucleus.examples.validate_vcf."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys



from absl.testing import absltest
import mock
from nucleus.examples import validate_vcf
from nucleus.protos import reference_pb2
from nucleus.protos import variants_pb2
from nucleus.testing import test_utils


class ValidateVcfTest(absltest.TestCase):

  def test_validate_contigs(self):
    contigs1 = [
        reference_pb2.ContigInfo(name='chr1'),
        reference_pb2.ContigInfo(name='chr2')]
    contigs2 = [
        reference_pb2.ContigInfo(name='ChrX'),
        reference_pb2.ContigInfo(name='ChrY')]

    with mock.patch.object(sys, 'exit') as mock_exit:
      validate_vcf.validate_contigs(contigs1, contigs1)
      mock_exit.assert_not_called()

    with mock.patch.object(sys, 'exit') as mock_exit:
      validate_vcf.validate_contigs(contigs2, contigs2)
      mock_exit.assert_not_called()

    with mock.patch.object(sys, 'exit') as mock_exit:
      validate_vcf.validate_contigs(contigs1, contigs2)
      mock_exit.assert_called_once_with(-1)

  def test_validate_variant(self):
    variant = variants_pb2.Variant(
        reference_name='chr1',
        start=5,
        end=10,
        reference_bases='AATTG')

    with mock.patch.object(sys, 'exit') as mock_exit:
      mock_ref = mock.MagicMock()
      mock_ref.query.return_value = 'AATTG'
      validate_vcf.validate_variant(mock_ref, variant)
      mock_exit.assert_not_called()

    with mock.patch.object(sys, 'exit') as mock_exit:
      mock_ref2 = mock.MagicMock()
      mock_ref2.query.return_value = 'CATAT'
      validate_vcf.validate_variant(mock_ref2, variant)
      mock_exit.assert_called_once_with(-1)

  def test_main(self):
    in_ref = test_utils.genomics_core_testdata('test.fasta')
    in_vcf = test_utils.genomics_core_testdata('test_phaseset.vcf')

    with mock.patch.object(sys, 'exit') as mock_exit:
      validate_vcf.main(['validate_vcf', in_ref, in_vcf])
      # Only the first call to sys.exit() matters.
      self.assertEqual(mock.call(-1), mock_exit.mock_calls[0])


if __name__ == '__main__':
  absltest.main()
