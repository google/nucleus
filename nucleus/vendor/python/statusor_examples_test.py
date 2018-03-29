# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from nucleus.vendor.python import statusor_examples


class StatusorClifWrapTest(absltest.TestCase):

  def test_make_int_ok(self):
    self.assertEqual(statusor_examples.MakeIntOK(), 42)

  def test_make_int_fail(self):
    with self.assertRaisesRegexp(ValueError, 'Invalid argument: MakeIntFail'):
      statusor_examples.MakeIntFail()

  def test_make_int_unique_ptr_ok(self):
    self.assertEqual(statusor_examples.MakeIntUniquePtrOK(), 421)

  def test_make_int_unique_ptr_fail(self):
    with self.assertRaisesRegexp(ValueError,
                                 'Invalid argument: MakeIntUniquePtrFail'):
      statusor_examples.MakeIntUniquePtrFail()

  def test_make_int_vector_ok(self):
    self.assertEqual(statusor_examples.MakeIntVectorOK(), [1, 2, 42])

  def test_make_int_vector_fail(self):
    with self.assertRaisesRegexp(ValueError,
                                 'Invalid argument: MakeIntVectorFail'):
      statusor_examples.MakeIntVectorFail()

  def test_returning_status_ok_returns_none(self):
    self.assertEqual(statusor_examples.FuncReturningStatusOK(), None)

  def test_returning_status_fail_raises(self):
    with self.assertRaisesRegexp(ValueError,
                                 'Invalid argument: FuncReturningStatusFail'):
      statusor_examples.FuncReturningStatusFail()


if __name__ == '__main__':
  absltest.main()
