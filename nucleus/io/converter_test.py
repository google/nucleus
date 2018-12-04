# Copyright 2018 Google LLC.
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
"""Tests for nucleus.examples.convert_genomics_file.

These tests do NOT establish the correctness of conversions---tests of the
fidelity of the Reader and Writer classes exist elsewhere in Nucleus.  Rather,
these tests simply exercise that the conversion *runs* for each input/output
file type.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import unittest

from absl import logging
from absl.testing import absltest
from absl.testing import parameterized
from nucleus.io import converter
from nucleus.testing import test_utils

basename = os.path.basename

# Initial (native) input files we will use to begin conversions.
ORIGINAL_TEST_FILES = [
    "test.bed", "test_reads.fastq", "test_features.gff", "test.sam",
    "test_sites.vcf"
]

# These formats require a header, so conversion from tfrecord to a native file
# format cannot be done faithfully.
FORMATS_REQUIRING_HEADER = [".bam", ".gff", ".sam", ".vcf"]


class ConvertGenomicsFileTest(parameterized.TestCase):

  def _convert(self, src, dest):
    logging.info("#### Converting: %s --> %s ... ", basename(src),
                 basename(dest))
    converter.convert(src, dest)

  @parameterized.parameters(*ORIGINAL_TEST_FILES)
  def test_conversion_to_tfrecord_and_back(self, original_input_file):
    """Test conversion from a native file format to tfrecord.gz, then back."""
    input_path = test_utils.genomics_core_testdata(original_input_file)
    tfrecord_output_path = test_utils.test_tmpfile(original_input_file +
                                                   ".tfrecord.gz")
    native_output_path = test_utils.test_tmpfile(original_input_file)

    # Test conversion from native format to tfrecord.
    self._convert(input_path, tfrecord_output_path)

    # TODO(b/63133103): remove this when SAM writer is implemented.
    if native_output_path.endswith(".sam"):
      raise unittest.SkipTest("SAM writing not yet supported")

    # Test conversion from tfrecord format back to native format.  Ensure that
    # conversions where we would need a header, but don't have one from the
    # input, trigger an error message.
    if any(
        native_output_path.endswith(ext) for ext in FORMATS_REQUIRING_HEADER):
      with self.assertRaisesRegexp(
          converter.ConversionError,
          "Input file does not have a header, which is needed to construct "
          "output file"):
        self._convert(tfrecord_output_path, native_output_path)

    else:
      self._convert(tfrecord_output_path, native_output_path)


if __name__ == "__main__":
  absltest.main()
 
