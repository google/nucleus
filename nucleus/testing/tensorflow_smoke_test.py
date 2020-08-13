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
"""Test that our Nucleus installation does not cause issues with TensorFlow."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest


class TensorflowIntegrationTest(absltest.TestCase):
  """Checks that Nucleus and TensorFlow interact well together."""

  def test_import_tf(self):
    """Checks that we can import TensorFlow."""
    # N.B. This test is only invoked when testing the pip package.
    import tensorflow


if __name__ == '__main__':
  absltest.main()
