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

"""Tests for nucleus.util.struct_utils."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys

from absl.testing import absltest
from absl.testing import parameterized
from nucleus.protos import struct_pb2
from nucleus.protos import variants_pb2
from nucleus.util import struct_utils


def _set_protomap_from_dict(d):
  """Returns a proto Map(str --> ListValue) with the given fields set.

  Args:
    d: dict(str --> list(Value)). The data to populate.

  Returns:
    The protocol buffer-defined Map(str --> ListValue).
  """
  # We use a Variant as an intermediate data structure since it contains the
  # desired output map types.
  v = variants_pb2.Variant()
  for key, values in d.items():
    v.info[key].values.extend(values)
  return v.info


def _wrapped_value_and_num(value):
  """Returns a list containing value plus the list's length."""
  if isinstance(value, (list, tuple)):
    return value, len(value)
  else:
    return [value], 1


class StructUtilsTest(parameterized.TestCase):

  @parameterized.parameters(
      dict(initial_fields={}, value=1, expected=[1]),
      dict(initial_fields={}, value=[1], expected=[1]),
      dict(initial_fields={}, value=[1, 2.5], expected=[1, 2.5]),
      dict(initial_fields={'field': []}, value=[1, 2.5], expected=[1, 2.5]),
      dict(
          initial_fields={'field': [struct_pb2.Value(number_value=5)]},
          value=[1, 2.5],
          expected=[5, 1, 2.5]),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(number_value=5),
                  struct_pb2.Value(number_value=-3.3),
              ]
          },
          value=[1, 2.5],
          expected=[5, -3.3, 1, 2.5]),
  )
  def test_add_number_field(self, initial_fields, value, expected):
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.add_number_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertEqual([v.number_value for v in field_map['field'].values],
                     expected)

  @parameterized.parameters(
      dict(initial_fields={}, value=1),
      dict(initial_fields={}, value=[1]),
      dict(initial_fields={}, value=[1, 2.5]),
      dict(initial_fields={'field': []}, value=[1, 2.5]),
      dict(
          initial_fields={'field': [struct_pb2.Value(number_value=5)]},
          value=[1, 2.5]),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(number_value=5),
                  struct_pb2.Value(number_value=-3.3),
              ]
          },
          value=[1, 2.5]),
  )
  def test_set_number_field(self, initial_fields, value):
    list_value, num_values = _wrapped_value_and_num(value)
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.set_number_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertLen(field_map['field'].values, num_values)
    self.assertEqual([v.number_value for v in field_map['field'].values],
                     list_value)

  @parameterized.parameters(
      dict(value=[], is_single_field=False, expected=[]),
      dict(value=[], is_single_field=True, expected=[]),
      dict(value=[1.5], is_single_field=False, expected=[1.5]),
      dict(value=[1.5], is_single_field=True, expected=1.5),
      dict(value=[1.5, 2], is_single_field=False, expected=[1.5, 2]),
      dict(value=[1.5, 2], is_single_field=True, expected=1.5),
  )
  def test_get_number_field(self, value, is_single_field, expected):
    key = 'field'
    field_map = _set_protomap_from_dict({})
    struct_utils.set_number_field(field_map, key, value)
    actual = struct_utils.get_number_field(field_map, key, is_single_field)
    self.assertEqual(actual, expected)

  @parameterized.parameters(
      dict(initial_fields={}, value=1, expected=[1]),
      dict(initial_fields={}, value=[1], expected=[1]),
      dict(initial_fields={}, value=[1, 2], expected=[1, 2]),
      dict(initial_fields={'field': []}, value=[1, 2], expected=[1, 2]),
      dict(
          initial_fields={'field': [struct_pb2.Value(int_value=5)]},
          value=[1, 2],
          expected=[5, 1, 2]),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(int_value=5),
                  struct_pb2.Value(int_value=-3),
              ]
          },
          value=[1, 2],
          expected=[5, -3, 1, 2]),
  )
  def test_add_int_field(self, initial_fields, value, expected):
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.add_int_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertEqual([v.int_value for v in field_map['field'].values], expected)

  @parameterized.parameters(
      dict(initial_fields={}, value=1),
      dict(initial_fields={}, value=[1]),
      dict(initial_fields={}, value=[1, 2]),
      dict(initial_fields={'field': []}, value=[1, 2]),
      dict(
          initial_fields={'field': [struct_pb2.Value(int_value=5)]},
          value=[1, 2]),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(int_value=5),
                  struct_pb2.Value(int_value=-3),
              ]
          },
          value=[1, 2]),
  )
  def test_set_int_field(self, initial_fields, value):
    list_value, num_values = _wrapped_value_and_num(value)
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.set_int_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertLen(field_map['field'].values, num_values)
    self.assertEqual([v.int_value for v in field_map['field'].values],
                     list_value)

  @parameterized.parameters(
      dict(value=[], is_single_field=False, expected=[]),
      dict(value=[], is_single_field=True, expected=[]),
      dict(value=[1], is_single_field=False, expected=[1]),
      dict(value=[1], is_single_field=True, expected=1),
      dict(value=[1, 2], is_single_field=False, expected=[1, 2]),
      dict(value=[1, 2], is_single_field=True, expected=1),
  )
  def test_get_int_field(self, value, is_single_field, expected):
    key = 'field'
    field_map = _set_protomap_from_dict({})
    struct_utils.set_int_field(field_map, key, value)
    actual = struct_utils.get_int_field(field_map, key, is_single_field)
    self.assertEqual(actual, expected)
    # Test long handling in Python 2
    if sys.version_info.major < 3:
      field_map = _set_protomap_from_dict({})
      struct_utils.set_int_field(field_map, key, [long(v) for v in value])
      actual = struct_utils.get_int_field(field_map, key, is_single_field)
      self.assertEqual(actual, expected)

  @parameterized.parameters(
      dict(initial_fields={}, value='hello', expected=['hello']),
      dict(initial_fields={}, value=['hello'], expected=['hello']),
      dict(initial_fields={}, value=['a', 'aah'], expected=['a', 'aah']),
      dict(
          initial_fields={'field': []},
          value=['bc', 'de'],
          expected=['bc', 'de']),
      dict(
          initial_fields={'field': [struct_pb2.Value(string_value='hi')]},
          value=['a', 'z'],
          expected=['hi', 'a', 'z']),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(string_value='abc'),
                  struct_pb2.Value(string_value=u'def'),
              ]
          },
          value=['ug', u'h'],
          expected=['abc', 'def', 'ug', 'h']),
  )
  def test_add_string_field(self, initial_fields, value, expected):
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.add_string_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertEqual([v.string_value for v in field_map['field'].values],
                     expected)

  @parameterized.parameters(
      dict(initial_fields={}, value='hello'),
      dict(initial_fields={}, value=['hello']),
      dict(initial_fields={}, value=['a', 'aah']),
      dict(initial_fields={'field': []}, value=['bc', 'de']),
      dict(
          initial_fields={'field': [struct_pb2.Value(string_value='hi')]},
          value=['a', 'z']),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(string_value='abc'),
                  struct_pb2.Value(string_value=u'def'),
              ]
          },
          value=['ug', u'h']),
  )
  def test_set_string_field(self, initial_fields, value):
    list_value, num_values = _wrapped_value_and_num(value)
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.set_string_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertLen(field_map['field'].values, num_values)
    self.assertEqual([v.string_value for v in field_map['field'].values],
                     list_value)

  @parameterized.parameters(
      dict(value=[], is_single_field=False, expected=[]),
      dict(value=[], is_single_field=True, expected=[]),
      dict(value=['hi'], is_single_field=False, expected=['hi']),
      dict(value=['single'], is_single_field=True, expected='single'),
      dict(value=['2', 'f'], is_single_field=False, expected=['2', 'f']),
      dict(value=['two', 'fields'], is_single_field=True, expected='two'),
  )
  def test_get_string_field(self, value, is_single_field, expected):
    key = 'field'
    field_map = _set_protomap_from_dict({})
    struct_utils.set_string_field(field_map, key, value)
    actual = struct_utils.get_string_field(field_map, key, is_single_field)
    self.assertEqual(actual, expected)

  @parameterized.parameters(
      dict(initial_fields={}, value=True, expected=[True]),
      dict(initial_fields={}, value=[True], expected=[True]),
      dict(initial_fields={}, value=[True, False], expected=[True, False]),
      dict(
          initial_fields={'field': []},
          value=[False, True],
          expected=[False, True]),
      dict(
          initial_fields={'field': [struct_pb2.Value(bool_value=True)]},
          value=[False, True],
          expected=[True, False, True]),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(bool_value=False),
                  struct_pb2.Value(bool_value=True),
              ]
          },
          value=[True, True],
          expected=[False, True, True, True]),
  )
  def test_add_bool_field(self, initial_fields, value, expected):
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.add_bool_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertEqual([v.bool_value for v in field_map['field'].values],
                     expected)

  @parameterized.parameters(
      dict(initial_fields={}, value=True),
      dict(initial_fields={}, value=[True]),
      dict(initial_fields={}, value=[False, True]),
      dict(initial_fields={'field': []}, value=[True, False]),
      dict(
          initial_fields={'field': [struct_pb2.Value(bool_value=True)]},
          value=[True, False]),
      dict(
          initial_fields={
              'field': [
                  struct_pb2.Value(bool_value=False),
                  struct_pb2.Value(bool_value=True),
              ]
          },
          value=[True, False]),
  )
  def test_set_bool_field(self, initial_fields, value):
    list_value, num_values = _wrapped_value_and_num(value)
    field_map = _set_protomap_from_dict(initial_fields)
    struct_utils.set_bool_field(field_map, 'field', value)
    self.assertIn('field', field_map)
    self.assertLen(field_map['field'].values, num_values)
    self.assertEqual([v.bool_value for v in field_map['field'].values],
                     list_value)

  @parameterized.parameters(
      dict(value=[], is_single_field=False, expected=[]),
      dict(value=[], is_single_field=True, expected=[]),
      dict(value=[True], is_single_field=False, expected=[True]),
      dict(value=[True], is_single_field=True, expected=True),
      dict(value=[True, False], is_single_field=False, expected=[True, False]),
      dict(value=[False, True], is_single_field=True, expected=False),
  )
  def test_get_bool_field(self, value, is_single_field, expected):
    key = 'field'
    field_map = _set_protomap_from_dict({})
    struct_utils.set_bool_field(field_map, key, value)
    actual = struct_utils.get_bool_field(field_map, key, is_single_field)
    self.assertEqual(actual, expected)


if __name__ == '__main__':
  absltest.main()
