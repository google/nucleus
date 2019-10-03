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

"""Constants related to the VCF variant specification.

See the full specification at https://samtools.github.io/hts-specs/VCFv4.3.pdf
for details.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import functools

from nucleus.protos import variants_pb2
from nucleus.util import struct_utils

# The alternate allele string for reference (no alt).
NO_ALT_ALLELE = '.'

# The alternate allele string for the gVCF "any" alternate allele.
GVCF_ALT_ALLELE = '<*>'

# Older symbolic alt allele, similar in meaning to gVCF alt allele
SYMBOLIC_ALT_ALLELE = '<NON_REF>'

# The replacement field used for missing data.
MISSING_FIELD = '.'

# Valid types for INFO and FORMAT fields, as per the VCF 4.3 spec.
CHARACTER_TYPE = 'Character'
FLAG_TYPE = 'Flag'
FLOAT_TYPE = 'Float'
INTEGER_TYPE = 'Integer'
STRING_TYPE = 'String'

# Reserved FILTER field definitions.
RESERVED_FILTER_FIELDS = [
    variants_pb2.VcfFilterInfo(id='PASS', description='All filters passed'),
]

# Reserved INFO field definitions, as per the VCF 4.3 spec.
RESERVED_INFO_FIELDS = [
    variants_pb2.VcfInfo(
        id='AA', number='1', type=STRING_TYPE, description='Ancestral allele'),
    variants_pb2.VcfInfo(
        id='AC',
        number='A',
        type=INTEGER_TYPE,
        description='Allele count in genotypes, for each ALT '
        'allele, in the same order as listed'),
    variants_pb2.VcfInfo(
        id='AD',
        number='R',
        type=INTEGER_TYPE,
        description='Total read depth for each allele'),
    variants_pb2.VcfInfo(
        id='ADF',
        number='R',
        type=INTEGER_TYPE,
        description='Read depth for each allele on the forward '
        'strand'),
    variants_pb2.VcfInfo(
        id='ADR',
        number='R',
        type=INTEGER_TYPE,
        description='Read depth for each allele on the reverse strand'),
    variants_pb2.VcfInfo(
        id='AF',
        number='A',
        type=FLOAT_TYPE,
        description='Allele frequency for each ALT allele in '
        'the same order as listed (estimated from '
        'primary data, not called genotypes)'),
    variants_pb2.VcfInfo(
        id='AN',
        number='1',
        type=INTEGER_TYPE,
        description='Total number of alleles in called genotypes'),
    variants_pb2.VcfInfo(
        id='BQ', number='1', type=FLOAT_TYPE, description='RMS base quality'),
    variants_pb2.VcfInfo(
        id='CIGAR',
        number='A',
        type=STRING_TYPE,
        description='Cigar string describing how to align an '
        'alternate allele to the reference allele'),
    variants_pb2.VcfInfo(
        id='DB', number='0', type=FLAG_TYPE, description='dbSNP membership'),
    variants_pb2.VcfInfo(
        id='DP',
        number='1',
        type=INTEGER_TYPE,
        description='Combined depth across samples'),
    variants_pb2.VcfInfo(
        id='END',
        number='1',
        type=INTEGER_TYPE,
        description='End position (for use with symbolic alleles)'),
    variants_pb2.VcfInfo(
        id='H2', number='0', type=FLAG_TYPE, description='HapMap2 membership'),
    variants_pb2.VcfInfo(
        id='H3', number='0', type=FLAG_TYPE, description='HapMap3 membership'),
    # NOTE: In the VCF 4.3 spec, the type of 'MQ' is listed as '.', even though
    # that is not specified as a valid type. Because root mean square is
    # typically a float value, we specify its type as FLOAT_TYPE.
    variants_pb2.VcfInfo(
        id='MQ', number='1', type=FLOAT_TYPE,
        description='RMS mapping quality'),
    variants_pb2.VcfInfo(
        id='MQ0',
        number='1',
        type=INTEGER_TYPE,
        description='Number of MAPQ == 0 reads'),
    variants_pb2.VcfInfo(
        id='NS',
        number='1',
        type=INTEGER_TYPE,
        description='Number of samples with data'),
    # NOTE: In the VCF 4.3 spec, the type of 'SB' is listed as '.', even though
    # that is not specified as a valid type. Because strand bias is usually a
    # numerical measurement (e.g. p-value of contingency table), we specify its
    # type as FLOAT_TYPE.
    variants_pb2.VcfInfo(
        id='SB', number='.', type=FLOAT_TYPE, description='Strand bias'),
    variants_pb2.VcfInfo(
        id='SOMATIC',
        number='0',
        type=FLAG_TYPE,
        description='Somatic mutation (for cancer genomics)'),
    variants_pb2.VcfInfo(
        id='VALIDATED',
        number='0',
        type=FLAG_TYPE,
        description='Validated by follow-up experiment'),
    variants_pb2.VcfInfo(
        id='1000G',
        number='0',
        type=FLAG_TYPE,
        description='1000 Genomes membership'),
]

# Reserved FORMAT field definitions, as per the VCF 4.3 spec.
RESERVED_FORMAT_FIELDS = [
    variants_pb2.VcfFormatInfo(
        id='AD',
        number='R',
        type=INTEGER_TYPE,
        description='Read depth for each allele'),
    variants_pb2.VcfFormatInfo(
        id='ADF',
        number='R',
        type=INTEGER_TYPE,
        description='Read depth for each allele on the '
        'forward strand'),
    variants_pb2.VcfFormatInfo(
        id='ADR',
        number='R',
        type=INTEGER_TYPE,
        description='Read depth for each allele on the '
        'reverse strand'),
    variants_pb2.VcfFormatInfo(
        id='DP', number='1', type=INTEGER_TYPE, description='Read depth'),
    variants_pb2.VcfFormatInfo(
        id='EC',
        number='A',
        type=INTEGER_TYPE,
        description='Expected alternate allele counts'),
    variants_pb2.VcfFormatInfo(
        id='FT',
        number='1',
        type=STRING_TYPE,
        description='Filter indicating if this genotype '
        'was "called"'),
    variants_pb2.VcfFormatInfo(
        id='GL',
        number='G',
        type=FLOAT_TYPE,
        description='Genotype likelihoods'),
    variants_pb2.VcfFormatInfo(
        id='GP',
        number='G',
        type=FLOAT_TYPE,
        description='Genotype posterior probabilities'),
    variants_pb2.VcfFormatInfo(
        id='GQ',
        number='1',
        type=INTEGER_TYPE,
        description='Conditional genotype quality'),
    variants_pb2.VcfFormatInfo(
        id='GT', number='1', type=STRING_TYPE, description='Genotype'),
    variants_pb2.VcfFormatInfo(
        id='HQ', number='2', type=INTEGER_TYPE,
        description='Haplotype quality'),
    variants_pb2.VcfFormatInfo(
        id='MQ',
        number='1',
        type=INTEGER_TYPE,
        description='RMS mapping quality'),
    variants_pb2.VcfFormatInfo(
        id='PL',
        number='G',
        type=INTEGER_TYPE,
        description='Phred-scaled genotype likelihoods '
        'rounded to the closest integer'),
    variants_pb2.VcfFormatInfo(
        id='PQ', number='1', type=INTEGER_TYPE, description='Phasing quality'),
    variants_pb2.VcfFormatInfo(
        id='PS', number='1', type=INTEGER_TYPE, description='Phase set'),
]

# Map from field type to the function used to set struct_pb2.Value elements
# of that type.
SET_FN_LOOKUP = {
    INTEGER_TYPE: struct_utils.set_int_field,
    FLOAT_TYPE: struct_utils.set_number_field,
    STRING_TYPE: struct_utils.set_string_field,
    CHARACTER_TYPE: struct_utils.set_string_field,
    FLAG_TYPE: struct_utils.set_bool_field,
}


def _get_reserved_field(field_id, reserved_fields):
  """Returns the desired reserved field.

  Args:
    field_id: str. The id of the field to retrieve.
    reserved_fields: list(fields). The reserved fields to search.

  Returns:
    The reserved field with the given `field_id`.

  Raises:
    ValueError: `field_id` is not a known reserved field.
  """
  matching_fields = [field for field in reserved_fields if field.id == field_id]
  if not matching_fields:
    raise ValueError('No reserved field with id `{}`'.format(field_id))
  return matching_fields[0]


def reserved_filter_field(field_id):
  """Returns the reserved FILTER field with the given ID."""
  return _get_reserved_field(field_id, RESERVED_FILTER_FIELDS)


def reserved_info_field(field_id):
  """Returns the reserved INFO field with the given ID."""
  return _get_reserved_field(field_id, RESERVED_INFO_FIELDS)


def reserved_format_field(field_id):
  """Returns the reserved FORMAT field with the given ID."""
  return _get_reserved_field(field_id, RESERVED_FORMAT_FIELDS)


def create_get_fn(value_type, number):
  """Returns a callable that extracts the typed information from a ListValue.

  Args:
    value_type: str. The value type stored as defined in the VCF 4.3 spec.
    number: str. The number of entries of this value as defined in the VCF spec.

  Returns:
    A callable that takes two inputs: A Map(str --> ListValue) and a string
    field name and returns the associated typed value(s). The return value is
    a list of typed values or a single typed value, depending on the expected
    number of values returned.
  """
  is_single_field = (number == '0' or number == '1')
  if value_type == CHARACTER_TYPE or value_type == STRING_TYPE:
    return functools.partial(
        struct_utils.get_string_field, is_single_field=is_single_field)
  elif value_type == INTEGER_TYPE:
    return functools.partial(
        struct_utils.get_int_field, is_single_field=is_single_field)
  elif value_type == FLOAT_TYPE:
    return functools.partial(
        struct_utils.get_number_field, is_single_field=is_single_field)
  elif value_type == FLAG_TYPE:
    return functools.partial(
        struct_utils.get_bool_field, is_single_field=is_single_field)
  else:
    raise ValueError('Invalid value_type: {}'.format(value_type))


# Map from INFO field name to the function used to set struct_pb2.Value elements
# of that field.
RESERVED_INFO_FIELD_SET_FNS = {
    info.id: SET_FN_LOOKUP[info.type]
    for info in RESERVED_INFO_FIELDS
}

# Map from INFO field name to the function used to get struct_pb2.Value elements
# of that field.
RESERVED_INFO_FIELD_GET_FNS = {
    info.id: create_get_fn(info.type, info.number)
    for info in RESERVED_INFO_FIELDS
}

# Map from FORMAT field name to the function used to set struct_pb2.Value
# elements of that field.
RESERVED_FORMAT_FIELD_SET_FNS = {
    fmt.id: SET_FN_LOOKUP[fmt.type]
    for fmt in RESERVED_FORMAT_FIELDS
}

# Map from FORMAT field name to the function used to get struct_pb2.Value
# elements of that field.
RESERVED_FORMAT_FIELD_GET_FNS = {
    fmt.id: create_get_fn(fmt.type, fmt.number)
    for fmt in RESERVED_FORMAT_FIELDS
}


def reserved_info_field_set_fn(field_name):
  """Returns the callable that sets the proper field for the given field_name.

  Args:
    field_name: str. The field name of the reserved INFO field (e.g. 'MQ').

  Returns:
    The callable that takes in a Map(str --> ListValue), field name, and value
    and modifies the map to populate the field_name entry with the given value.

  Raises:
    ValueError: The field_name is not a known reserved INFO field.
  """
  try:
    return RESERVED_INFO_FIELD_SET_FNS[field_name]
  except KeyError:
    raise ValueError('Unknown reserved INFO field: {}'.format(field_name))


def reserved_info_field_get_fn(field_name):
  """Returns the callable that gets the proper field for the given field_name.

  Args:
    field_name: str. The field name of the reserved INFO field (e.g. 'MQ').

  Returns:
    The callable that takes in a Map(str --> ListValue), and field name and
    returns the associated typed value(s).

  Raises:
    ValueError: The field_name is not a known reserved INFO field.
  """
  try:
    return RESERVED_INFO_FIELD_GET_FNS[field_name]
  except KeyError:
    raise ValueError(
        'Unknown reserved INFO field to get: {}'.format(field_name))


def reserved_format_field_set_fn(field_name):
  """Returns the callable that sets the proper field for the given field_name.

  Args:
    field_name: str. The field name of the reserved FORMAT field (e.g. 'AD').

  Returns:
    The callable that takes in a Map(str --> ListValue), field name, and value
    and modifies the map to populate the field_name entry with the given value.

  Raises:
    ValueError: The field_name is not a known reserved FORMAT field.
  """
  try:
    return RESERVED_FORMAT_FIELD_SET_FNS[field_name]
  except KeyError:
    raise ValueError('Unknown reserved FORMAT field: {}'.format(field_name))


def reserved_format_field_get_fn(field_name):
  """Returns the callable that gets the proper field for the given field_name.

  Args:
    field_name: str. The field name of the reserved FORMAT field (e.g. 'AD').

  Returns:
    The callable that takes in a Map(str --> ListValue), and field name and
    returns the associated typed value(s).

  Raises:
    ValueError: The field_name is not a known reserved FORMAT field.
  """
  try:
    return RESERVED_FORMAT_FIELD_GET_FNS[field_name]
  except KeyError:
    raise ValueError(
        'Unknown reserved FORMAT field to get: {}'.format(field_name))
