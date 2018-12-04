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
"""Library of application-specific errors.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import contextlib
import errno
import sys

from absl import logging


class Error(Exception):
  """Base class for core error types."""


class CommandLineError(Error):
  """Exception class related to invalid command-line flags."""


def log_and_raise(msg, exception_class=Error):
  """Logs the given message at ERROR level and raises exception.

  Args:
    msg: [`string`]. The message to log and use in the raised exception.
    exception_class: [`Exception`]. The class of exception to raise.

  Raises:
    Error: An exception of the type specified by the input exception_class.
  """
  logging.error(msg)
  raise exception_class(msg)


@contextlib.contextmanager
def clean_commandline_error_exit(
    allowed_exceptions=(Error, CommandLineError), exit_value=errno.ENOENT):
  """Wraps commands to capture certain exceptions and exit without stacktraces.

  This function is intended to wrap all code within main() of Python binaries
  to provide a mechanism for user errors to exit abnormally without causing
  exceptions to be thrown. Any exceptions that are subclasses of those listed
  in `allowed_exceptions` will be caught and the program will quietly exit with
  `exit_value`. Other exceptions are propagated normally.

  NOTE: This function should only be used as a context manager and its usage
  should be limited to main().

  Args:
    allowed_exceptions: [`tuple of Exception`]. A tuple of Exception classes
      that should not be raised, but instead quietly caused to exit the program.
    exit_value: [`int`]. The value to return upon program exit.

  Yields:
    The yield in this function is used to allow the block nested in the "with"
    statement to be executed.
  """
  try:
    yield
  except allowed_exceptions:
    sys.exit(exit_value)
