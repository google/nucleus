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
"""Classes for reading FASTA files.

The FASTA format is described at
https://en.wikipedia.org/wiki/FASTA_format

API for reading:

```python
from nucleus.io import fasta
from nucleus.protos import range_pb2

with fasta.IndexedFastaReader(input_path) as reader:
  region = range_pb2.Range(reference_name='chrM', start=1, end=6)
  basepair_string = reader.query(region)
  print(basepair_string)
```

If `input_path` ends with '.gz', it is assumed to be compressed.  All FASTA
files are assumed to be indexed with the index file located at
`input_path + '.fai'`.
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import collections

import six

from nucleus.io import gfile
from nucleus.io import genomics_reader
from nucleus.io.python import reference
from nucleus.protos import fasta_pb2
from nucleus.protos import reference_pb2
from nucleus.util import ranges

# TODO(thomaswc): Replace this with a real protocol buffer definition.
RefFastaHeader = collections.namedtuple(
    'RefFastaHeader', ['contigs'])


class FastaReader(genomics_reader.DispatchingGenomicsReader):
  """Class for reading (name, bases) tuples from FASTA files."""

  def _native_reader(self, input_path, **kwargs):
    fai_path = input_path + '.fai'
    if gfile.Exists(fai_path):
      return IndexedFastaReader(input_path, **kwargs)
    return UnindexedFastaReader(input_path, **kwargs)

  def _record_proto(self):
    return fasta_pb2.FastaRecord


class IndexedFastaReader(genomics_reader.GenomicsReader):
  """Class for reading from FASTA files containing a reference genome."""

  def __init__(self, input_path, keep_true_case=False, cache_size=None):
    """Initializes an IndexedFastaReader.

    Args:
      input_path: string. A path to a resource containing FASTA records.
      keep_true_case: bool. If False, casts all bases to uppercase before
        returning them.
      cache_size: integer. Number of bases to cache from previous queries.
        Defaults to 64K.  The cache can be disabled using cache_size=0.
    """
    super(IndexedFastaReader, self).__init__()

    options = fasta_pb2.FastaReaderOptions(keep_true_case=keep_true_case)

    fasta_path = input_path
    fai_path = fasta_path + '.fai'
    if cache_size is None:
      # Use the C++-defined default cache size.
      self._reader = reference.IndexedFastaReader.from_file(
          fasta_path, fai_path, options)
    else:
      self._reader = reference.IndexedFastaReader.from_file(
          fasta_path, fai_path, options, cache_size)

    # TODO(thomaswc): Define a RefFastaHeader proto, and use it instead of this.
    self.header = RefFastaHeader(contigs=self._reader.contigs)

  def iterate(self):
    """Returns an iterable of (name, bases) tuples contained in this file."""
    return self._reader.iterate()

  def query(self, region):
    """Returns the base pairs (as a string) in the given region."""
    return self._reader.bases(region)

  def is_valid(self, region):
    """Returns whether the region is contained in this FASTA file."""
    return self._reader.is_valid_interval(region)

  def contig(self, contig_name):
    """Returns a ContigInfo proto for contig_name."""
    return self._reader.contig(contig_name)

  @property
  def c_reader(self):
    """Returns the underlying C++ reader."""
    return self._reader

  def __exit__(self, exit_type, exit_value, exit_traceback):
    self._reader.__exit__(exit_type, exit_value, exit_traceback)


class UnindexedFastaReader(genomics_reader.GenomicsReader):
  """Class for reading from unindexed FASTA files."""

  def __init__(self, input_path):
    """Initializes an UnindexedFastaReader.

    Args:
      input_path: string. A path to a resource containing FASTA records.
    """
    super(UnindexedFastaReader, self).__init__()

    self._reader = reference.UnindexedFastaReader.from_file(input_path)

  def iterate(self):
    """Returns an iterable of (name, bases) tuples contained in this file."""
    return self._reader.iterate()

  def query(self, region):
    """Returns the base pairs (as a string) in the given region."""
    raise NotImplementedError('Can not query an unindexed FASTA file')

  def is_valid(self, region):
    """Returns whether the region is contained in this FASTA file."""
    return self._reader.is_valid_interval(region)

  def contig(self, contig_name):
    """Returns a ContigInfo proto for contig_name."""
    raise NotImplementedError('Contigs unknown for an unindexed FASTA file')

  @property
  def c_reader(self):
    """Returns the underlying C++ reader."""
    return self._reader

  def __exit__(self, exit_type, exit_value, exit_traceback):
    self._reader.__exit__(exit_type, exit_value, exit_traceback)


class InMemoryFastaReader(genomics_reader.GenomicsReader):
  """An `IndexedFastaReader` getting its bases from an in-memory data structure.

  An `InMemoryFastaReader` provides the same API as `IndexedFastaReader` but
  doesn't fetch its data from an on-disk FASTA file but rather fetches the bases
  from an in-memory cache containing (chromosome, start, bases) tuples.

  In particular, the `query(Range(chrom, start, end))` operation fetches bases
  from the tuple where `chrom` == chromosome, and then from the bases where the
  first base of bases starts at start. If start > 0, then the bases string is
  assumed to contain bases starting from that position in the region. For
  example, the record ('1', 10, 'ACGT') implies that
  `query(ranges.make_range('1', 11, 12))` will return the base 'C', as the 'A'
  base is at position 10. This makes it straightforward to cache a small region
  of a full chromosome without having to store the entire chromosome sequence in
  memory (potentially big!).
  """

  def __init__(self, chromosomes):
    """Initializes an InMemoryFastaReader using data from chromosomes.

    Args:
      chromosomes: list[tuple]. The chromosomes we are caching in memory as a
        list of tuples. Each tuple must be exactly three elements in length,
        containing (chromosome name [str], start [int], bases [str]).

    Raises:
      ValueError: If any of the chromosomes tuples are invalid.
    """
    super(InMemoryFastaReader, self).__init__()

    ref_seqs = []
    contigs = []
    for i, (contig_name, start, bases) in enumerate(chromosomes):
      if start < 0:
        raise ValueError('start={} must be >= for chromosome={}'.format(
            start, contig_name))
      if not bases:
        raise ValueError(
            'Bases must contain at least one base, but got "{}"'.format(bases))

      end = start + len(bases)
      ref_seqs.append(reference_pb2.ReferenceSequence(
          region=ranges.make_range(contig_name, start, end), bases=bases))
      contigs.append(
          reference_pb2.ContigInfo(
              name=contig_name, n_bases=end, pos_in_fasta=i))

    self._reader = reference.InMemoryFastaReader.create(contigs, ref_seqs)
    self.header = RefFastaHeader(contigs=self._reader.contigs)

  def iterate(self):
    """Returns an iterable of (name, bases) tuples contained in this file."""
    return self._reader.iterate()

  def query(self, region):
    """Returns the base pairs (as a string) in the given region."""
    return self._reader.bases(region)

  def is_valid(self, region):
    """Returns whether the region is contained in this FASTA file."""
    return self._reader.is_valid_interval(region)

  def contig(self, contig_name):
    """Returns a ContigInfo proto for contig_name."""
    return self._reader.contig(contig_name)

  @property
  def c_reader(self):
    """Returns the underlying C++ reader."""
    return self._reader

  def __str__(self):

    def _format_refseq(refseq):
      bases = refseq.bases
      if len(bases) >= 50:
        bases = bases[0:50] + '...'
      return 'Contig(chrom={} start={}, end={}, bases={})'.format(
          refseq.region.reference_name, refseq.region.start, refseq.region.end,
          bases)

    contigs_strs = [
        _format_refseq(refseq)
        for refseq in six.itervalues(self._reader.reference_sequences)
    ]
    return 'InMemoryFastaReader(contigs={})'.format(''.join(contigs_strs))

  __repr__ = __str__
