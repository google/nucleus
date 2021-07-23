# Copyright 2020 Google LLC.
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
"""Beam sources for genomics file formats."""

from nucleus.io import bed
from nucleus.io import sam

# pylint:disable=g-bad-import-order
# Beam needs to be imported after nucleus for this to work in open source.
from apache_beam.io import filebasedsource
from apache_beam.io.iobase import Read
from apache_beam.transforms import PTransform
# pylint:enable=g-bad-import-order


class _GenomicsSource(filebasedsource.FileBasedSource):
  """A base source for reading genomics files.

  Do not use this class directly. Instead, use the subclass for the specific
  file type.
  """

  def __init__(self, file_pattern, validate, **nucleus_kwargs):
    """Initializes a _GenomicsSource for use with readers for genomics files."""

    super(_GenomicsSource, self).__init__(
        file_pattern=file_pattern, splittable=False, validate=validate)
    self.nucleus_kwargs = nucleus_kwargs

  def read_records(self, input_path, offset_range_tracker):
    """Yields records returned by nucleus_reader."""
    if offset_range_tracker.start_position():
      raise ValueError('Start position not 0: %d' %
                       offset_range_tracker.start_position())
    current_offset = offset_range_tracker.start_position()
    reader = self.nucleus_reader(input_path, **self.nucleus_kwargs)

    with reader:
      for record in reader:
        if not offset_range_tracker.try_claim(current_offset):
          raise RuntimeError('Unable to claim position: %d' % current_offset)
        yield record
        current_offset += 1

  @property
  def nucleus_reader(self):
    raise NotImplementedError


class _BedSource(_GenomicsSource):
  """A source for reading BED files."""

  nucleus_reader = bed.BedReader


class _SamSource(_GenomicsSource):
  """A source for reading SAM/BAM files."""

  nucleus_reader = sam.SamReader


class ReadGenomicsFile(PTransform):
  """For reading one or more genomics files.

  Do not use this class directly. Instead, use the subclass for the specific
  file type.
  """

  def __init__(self, file_pattern, validate=True, **nucleus_kwargs):
    """Initializes the ReadSam transform."""

    super(ReadGenomicsFile, self).__init__()
    self._source = self._source_class(
        file_pattern, validate=validate, **nucleus_kwargs)

  def expand(self, pvalue):
    return pvalue.pipeline | Read(self._source)

  @property
  def _source_class(self):
    raise NotImplementedError


class ReadBed(ReadGenomicsFile):
  """For reading records from one or more BED files."""

  _source_class = _BedSource


class ReadSam(ReadGenomicsFile):
  """For reading records from one or more SAM/BAM files."""

  _source_class = _SamSource
