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
"""Utility functions for working with alignment CIGAR operations.

The CIGAR format is defined within the SAM spec, available at
https://samtools.github.io/hts-specs/SAMv1.pdf

This module provides utility functions for interacting with the parsed
representations of CIGAR strings.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import re
import six
from nucleus.protos import cigar_pb2

# A frozenset of all CigarUnit.Operation enum values that advance the alignment
# with respect to the reference genome and read, respectively.
REF_ADVANCING_OPS = frozenset([
    cigar_pb2.CigarUnit.ALIGNMENT_MATCH,
    cigar_pb2.CigarUnit.SEQUENCE_MATCH,
    cigar_pb2.CigarUnit.DELETE,
    cigar_pb2.CigarUnit.SKIP,
    cigar_pb2.CigarUnit.SEQUENCE_MISMATCH
])
READ_ADVANCING_OPS = frozenset([
    cigar_pb2.CigarUnit.ALIGNMENT_MATCH,
    cigar_pb2.CigarUnit.SEQUENCE_MATCH,
    cigar_pb2.CigarUnit.INSERT,
    cigar_pb2.CigarUnit.CLIP_SOFT,
    cigar_pb2.CigarUnit.SEQUENCE_MISMATCH
])

# A map from CigarUnit.Operation (e.g., CigarUnit.ALIGNMENT_MATCH) enum values
# to their corresponding single character cigar codes (e.g., 'M').
CIGAR_OPS_TO_CHAR = {
    cigar_pb2.CigarUnit.ALIGNMENT_MATCH: 'M',
    cigar_pb2.CigarUnit.INSERT: 'I',
    cigar_pb2.CigarUnit.DELETE: 'D',
    cigar_pb2.CigarUnit.SKIP: 'N',
    cigar_pb2.CigarUnit.CLIP_SOFT: 'S',
    cigar_pb2.CigarUnit.CLIP_HARD: 'H',
    cigar_pb2.CigarUnit.PAD: 'P',
    cigar_pb2.CigarUnit.SEQUENCE_MATCH: '=',
    cigar_pb2.CigarUnit.SEQUENCE_MISMATCH: 'X',
}

# A map from single character cigar codes (e.g., 'M') to their corresponding
# CigarUnit.Operation (e.g., CigarUnit.ALIGNMENT_MATCH) enum values.
CHAR_TO_CIGAR_OPS = {v: k for k, v in CIGAR_OPS_TO_CHAR.items()}

# All of the CigarUnit.Operation values in a frozen set.
ALL_CIGAR_OPS = frozenset(CIGAR_OPS_TO_CHAR.keys())

# Regular expression that matches only valid full cigar strings.
VALID_CIGAR_RE = re.compile(
    r'^(\d+[' + ''.join(CHAR_TO_CIGAR_OPS.keys()) + '])+$')

# Regular expression that matches a single len/op cigar element. The match is
# grouped, so CIGAR_STR_SPLITTER_RE.finditer(cigar_str) returns grouped units
# of the cigar string in order.
CIGAR_STR_SPLITTER_RE = re.compile(
    r'(\d+[' + ''.join(CHAR_TO_CIGAR_OPS.keys()) + '])')


def format_cigar_units(cigar_units):
  """Returns the string version of an iterable of CigarUnit protos.

  Args:
    cigar_units: iterable[CigarUnit] protos.

  Returns:
    A string representation of the CigarUnit protos that conforms to the
    CIGAR string specification.
  """
  return ''.join(
      str(unit.operation_length) + CIGAR_OPS_TO_CHAR[unit.operation]
      for unit in cigar_units)


def parse_cigar_string(cigar_str):
  """Parse a cigar string into a list of cigar units.

  For example, if cigar_str is 150M2S, this function will return:

  [
    CigarUnit(operation=ALIGNMENT_MATCH, operation_length=150),
    CigarUnit(operation=SOFT_CLIP, operation_length=2)
  ]

  Args:
    cigar_str: str containing a valid cigar.

  Returns:
    list[cigar_pb2.CigarUnit].

  Raises:
    ValueError: If cigar_str isn't a well-formed CIGAR.
  """
  if not cigar_str:
    raise ValueError('cigar_str cannot be empty')
  if not VALID_CIGAR_RE.match(cigar_str):
    raise ValueError('Malformed CIGAR string {}'.format(cigar_str))
  parts = CIGAR_STR_SPLITTER_RE.finditer(cigar_str)
  return [to_cigar_unit(part.group(1)) for part in parts]


def alignment_length(cigar_units):
  """Computes the span in basepairs of the cigar units.

  Args:
    cigar_units: iterable[CigarUnit] whose alignment length we want to compute.

  Returns:
    The number of basepairs spanned by the cigar_units.
  """
  return sum(unit.operation_length
             for unit in cigar_units
             if unit.operation in REF_ADVANCING_OPS)


def to_cigar_unit(source):
  """Creates a cigar_pb2 CigarUnit from source.

  This function attempts to convert source into a CigarUnit protobuf. If
  source is a string, it must be a single CIGAR string specification like
  '12M'. If source is a tuple or a list, must have exactly two elements
  (operation_length, opstr). operation_length can be a string or int, and must
  be >= 1. opstr should be a single character CIGAR specification (e.g., 'M').
  If source is already a CigarUnit, it is just passed through unmodified.

  Args:
    source: many types allowed. The object we want to convert to a CigarUnit
      proto.

  Returns:
    CigarUnit proto with operation_length and operation set to values from
      source.

  Raises:
    ValueError: if source cannot be converted or is malformed.
  """
  try:
    if isinstance(source, cigar_pb2.CigarUnit):
      return source
    elif isinstance(source, six.string_types):
      l, op = source[:-1], source[-1]
    elif isinstance(source, (tuple, list)):
      l, op = source
    else:
      raise ValueError('Unexpected source', source)

    if isinstance(op, six.string_types):
      op = CHAR_TO_CIGAR_OPS[op]
    l = int(l)
    if l < 1:
      raise ValueError('Length must be >= 1', l)
    return cigar_pb2.CigarUnit(operation=op, operation_length=int(l))
  except (KeyError, IndexError):
    raise ValueError('Failed to convert {} into a CigarUnit'.format(source))


def to_cigar_units(source):
  """Converts object to a list of CigarUnit.

  This function attempts to convert source into a list of CigarUnit protos.
  If source is a string, we assume it is a CIGAR string and call
  parse_cigar_string on it, returning the result. It not, we assume it's an
  iterable containing element to be converted with to_cigar_unit(). The
  resulting list of converted elements is returned.

  Args:
    source: str or iterable to convert to CigarUnit protos.

  Returns:
    list[CigarUnit].
  """
  if isinstance(source, six.string_types):
    return parse_cigar_string(source)
  else:
    return [to_cigar_unit(singleton) for singleton in source]
