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

"""Tests for genomics_io's plugin system."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.io import sam


class PluginTest(absltest.TestCase):
  """Test that the plugin loads correctly."""

  def test_tfbam_plugin_loads(self):
    reader = sam.SamReader('mouse@25.tfbam', use_index=True)
    self.assertIsNotNone(reader)


if __name__ == '__main__':
  absltest.main()
