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

"""Tests for hts_verbose."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.io.python import hts_verbose


class HtsVerbose(absltest.TestCase):

  def test_set(self):
    hts_verbose.set(hts_verbose.htsLogLevel.HTS_LOG_TRACE)
    level = hts_verbose.get()
    self.assertEqual(level, hts_verbose.htsLogLevel.HTS_LOG_TRACE)

    hts_verbose.set(hts_verbose.htsLogLevel.HTS_LOG_INFO)
    level = hts_verbose.get()
    self.assertEqual(level, hts_verbose.htsLogLevel.HTS_LOG_INFO)

    hts_verbose.set(hts_verbose.htsLogLevel['HTS_LOG_DEBUG'])
    level = hts_verbose.get()
    self.assertEqual(level, hts_verbose.htsLogLevel.HTS_LOG_DEBUG)


if __name__ == '__main__':
  absltest.main()
