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

"""Tests for nucleus.util.errors."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import errno
import sys

from absl import logging
from absl.testing import absltest
from absl.testing import parameterized
import mock
from nucleus.util import errors


class ErrorsTest(parameterized.TestCase):

  @parameterized.parameters(
      ('empty flag', errors.CommandLineError),
      ('bad value', ValueError),
      ('base error', errors.Error),
  )
  def test_log_and_raise(self, msg, cls):
    with mock.patch.object(logging, 'error') as mock_logging:
      with self.assertRaisesRegexp(cls, msg):
        errors.log_and_raise(msg, cls)
      mock_logging.assert_called_once_with(msg)

  @parameterized.parameters(
      (ValueError, 'ValueError exception'),
      (IOError, 'IOError exception'),
  )
  def test_clean_commandline_error_exit_raise_non_allowed(self, exc_type, msg):
    with self.assertRaisesRegexp(exc_type, msg):
      with errors.clean_commandline_error_exit():
        raise exc_type(msg)

  @parameterized.parameters(
      (errors.CommandLineError, errno.ENOENT),
      (errors.Error, errno.EINVAL),
  )
  def test_clean_commandline_error_exit_clean_exit(self, exc_type, exit_value):
    with mock.patch.object(sys, 'exit') as mock_exit:
      with errors.clean_commandline_error_exit(exit_value=exit_value):
        raise exc_type()
    mock_exit.assert_called_once_with(exit_value)


if __name__ == '__main__':
  absltest.main()
