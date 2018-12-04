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

"""Test that our protobuf implementation behaves as we'd expect."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from google.protobuf.internal import api_implementation
# This next import is unused, but we are testing that any program
# which includes a Nucleus library uses the cpp protobuf
# implementation.
# pylint: disable=unused-import
from nucleus.io import sam


class ProtobufImplementationTest(absltest.TestCase):
  """Checks that our protobufs have the properties we expect."""

  def test_protobuf_uses_fast_cpp(self):
    """Checks that we are using the fast cpp version of python protobufs."""
    self.assertEqual(api_implementation.Type(), 'cpp')


if __name__ == '__main__':
  absltest.main()
