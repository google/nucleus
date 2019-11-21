# nucleus.io.fasta -- Classes for reading FASTA files.
**Source code:** [nucleus/io/fasta.py](https://github.com/google/nucleus/tree/master/nucleus/io/fasta.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
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

## Classes overview
Name | Description
-----|------------
[`FastaReader`](#fastareader) | Class for reading (name, bases) tuples from FASTA files.
[`InMemoryFastaReader`](#inmemoryfastareader) | An `IndexedFastaReader` getting its bases from an in-memory data structure.
[`IndexedFastaReader`](#indexedfastareader) | Class for reading from FASTA files containing a reference genome.
[`UnindexedFastaReader`](#unindexedfastareader) | Class for reading from unindexed FASTA files.

## Classes
### FastaReader
```
Class for reading (name, bases) tuples from FASTA files.
```

### InMemoryFastaReader
```
An `IndexedFastaReader` getting its bases from an in-memory data structure.

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
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, chromosomes)`
```
Initializes an InMemoryFastaReader using data from chromosomes.

Args:
  chromosomes: list[tuple]. The chromosomes we are caching in memory as a
    list of tuples. Each tuple must be exactly three elements in length,
    containing (chromosome name [str], start [int], bases [str]).

Raises:
  ValueError: If any of the chromosomes tuples are invalid.
```

<a name="c_reader"></a>
##### `c_reader(self)`
```
Returns the underlying C++ reader.
```

<a name="contig"></a>
##### `contig(self, contig_name)`
```
Returns a ContigInfo proto for contig_name.
```

<a name="is_valid"></a>
##### `is_valid(self, region)`
```
Returns whether the region is contained in this FASTA file.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of (name, bases) tuples contained in this file.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns the base pairs (as a string) in the given region.
```

### IndexedFastaReader
```
Class for reading from FASTA files containing a reference genome.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, keep_true_case=False, cache_size=None)`
```
Initializes an IndexedFastaReader.

Args:
  input_path: string. A path to a resource containing FASTA records.
  keep_true_case: bool. If False, casts all bases to uppercase before
    returning them.
  cache_size: integer. Number of bases to cache from previous queries.
    Defaults to 64K.  The cache can be disabled using cache_size=0.
```

<a name="c_reader"></a>
##### `c_reader(self)`
```
Returns the underlying C++ reader.
```

<a name="contig"></a>
##### `contig(self, contig_name)`
```
Returns a ContigInfo proto for contig_name.
```

<a name="is_valid"></a>
##### `is_valid(self, region)`
```
Returns whether the region is contained in this FASTA file.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of (name, bases) tuples contained in this file.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns the base pairs (as a string) in the given region.
```

### UnindexedFastaReader
```
Class for reading from unindexed FASTA files.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path)`
```
Initializes an UnindexedFastaReader.

Args:
  input_path: string. A path to a resource containing FASTA records.
```

<a name="c_reader"></a>
##### `c_reader(self)`
```
Returns the underlying C++ reader.
```

<a name="contig"></a>
##### `contig(self, contig_name)`
```
Returns a ContigInfo proto for contig_name.
```

<a name="is_valid"></a>
##### `is_valid(self, region)`
```
Returns whether the region is contained in this FASTA file.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of (name, bases) tuples contained in this file.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns the base pairs (as a string) in the given region.
```

