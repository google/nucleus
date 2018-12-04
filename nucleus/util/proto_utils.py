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
"""Utility library for working with protobufs."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from google.protobuf.internal import api_implementation


def uses_fast_cpp_protos_or_die():
  """Raises an error if a slow protobuf implementation is being used."""
  if api_implementation.Type() != 'cpp':
    raise ValueError('Expected to be using C++ protobuf implementation '
                     '(api_implementation.Type() == "cpp") but it is {}'.format(
                         api_implementation.Type()))
