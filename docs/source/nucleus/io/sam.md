# nucleus.io.sam -- Classes for reading and writing SAM and BAM files.
**Source code:** [nucleus/io/sam.py](https://github.com/google/nucleus/tree/master/nucleus/io/sam.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
The SAM/BAM/CRAM formats are described at
https://samtools.github.io/hts-specs/SAMv1.pdf
https://samtools.github.io/hts-specs/CRAMv3.pdf

API for reading:

```python
from nucleus.io import sam

with sam.SamReader(input_path) as reader:
  for read in reader:
    print(read)
```

where `read` is a `nucleus.genomics.v1.Read` protocol buffer. input_path will
dynamically decode the underlying records depending the file extension, with
`.sam` for SAM files, `.bam` for BAM files, and `.cram` for CRAM files. It will
also search for an appropriate index file to use to enable calls to the
`query()` method.

API for writing SAM/BAM:

```python
from nucleus.io import sam

# reads is an iterable of nucleus.genomics.v1.Read protocol buffers.
reads = ...

with sam.SamWriter(output_path, header=header) as writer:
  for read in reads:
    writer.write(read)
```

API for writing CRAM:

```python
# ref_path is required for writing CRAM files. If embed_ref, the output CRAM
# file will embed reference sequences.
with sam.SamWriter(output_path, header=header, ref_path=ref_path,
                   embed_ref=embed_ref) as writer:
  for read in reads:
    writer.write(read)
```

For both reading and writing, if the path provided to the constructor contains
'.tfrecord' as an extension, a `TFRecord` file is assumed and attempted to be
read or written. Otherwise, the filename is treated as a true SAM/BAM/CRAM file.

For `TFRecord` files, ending in a '.gz' suffix causes the file to be treated as
compressed with gzip.

Notes on using CRAM with SamReader
--------------------------------

Nucleus supports reading from CRAM files using the same API as for SAM/BAM:

```python
from nucleus.io import sam

with sam.SamReader("/path/to/sample.cram") as reader:
  for read in reader:
    print(read)
```

There is one type of CRAM file, though, that has a slightly more complicated
API. If the CRAM file uses read sequence compression with an external reference
file, and this reference file is no longer accessible in the location specified
by the CRAM file's "UR" tag and cannot be found in the local genome cache, its
location must be passed to SamReader via the ref_path parameter:

```python
from nucleus.io import sam

cram_path = "/path/to/sample.cram"
ref_path = "/path/to/genome.fasta"
with sam.SamReader(cram_path, ref_path=ref_path) as reader:
  for read in reader:
    print(read)
```

Unfortunately, htslib is unable to load the ref_path from anything other than a
POSIX filesystem. (htslib plugin filesystems like S3 or GCS buckets won't work).
For that reason, we don't recommend the use of CRAM files with external
reference files, but instead suggest using read sequence compression with
embedded reference data. (This has a minor impact on file size, but
significantly improves file access simplicity and safety.)

For more information about CRAM, see:
* The `samtools` documentation at http://www.htslib.org/doc/samtools.html
* The "Global Options" section of the samtools docs at http://www.htslib.org/doc/samtools.html#GLOBAL_OPTIONS
* How reference sequences are encoded in CRAM at http://www.htslib.org/doc/samtools.html#REFERENCE_SEQUENCES
* Finally, benchmarking of different CRAM options http://www.htslib.org/benchmarks/CRAM.html

## Classes overview
Name | Description
-----|------------
[`InMemorySamReader`](#inmemorysamreader) | Python interface class for in-memory SAM/BAM/CRAM reader.
[`NativeSamReader`](#nativesamreader) | Class for reading from native SAM/BAM/CRAM files.
[`NativeSamWriter`](#nativesamwriter) | Class for writing to native SAM/BAM/CRAM files.
[`SamReader`](#samreader) | Class for reading Read protos from SAM/BAM/CRAM or TFRecord files.
[`SamWriter`](#samwriter) | Class for writing Read protos to SAM or TFRecord files.

## Classes
### InMemorySamReader
```
Python interface class for in-memory SAM/BAM/CRAM reader.

Attributes:
  reads: list[nucleus.genomics.v1.Read]. The list of in-memory reads.
  is_sorted: bool, True if reads are sorted.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, reads, is_sorted=False)`


<a name="iterate"></a>
##### `iterate(self)`
```
Iterate over all records in the reads.

Returns:
  An iterator over nucleus.genomics.v1.Read's.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the reads in the region.

Args:
  region: nucleus.genomics.v1.Range. The query region.

Returns:
  An iterator over nucleus.genomics.v1.Read protos.
```

<a name="replace_reads"></a>
##### `replace_reads(self, reads, is_sorted=False)`
```
Replace the reads stored by this reader.
```

### NativeSamReader
```
Class for reading from native SAM/BAM/CRAM files.

Most users will want to use SamReader instead, because it dynamically
dispatches between reading native SAM/BAM/CRAM files and TFRecord files based
on the filename's extensions.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, ref_path=None, read_requirements=None, parse_aux_fields=False, hts_block_size=None, downsample_fraction=None, random_seed=None, use_original_base_quality_scores=False)`
```
Initializes a NativeSamReader.

Args:
  input_path: str. A path to a resource containing SAM/BAM/CRAM records.
    Currently supports SAM text format, BAM binary format, and CRAM.
  ref_path: optional str or None. Only used for CRAM decoding, and only
    necessary if the UR encoded path in the CRAM itself needs to be
    overridden. If provided, we will tell the CRAM decoder to use this FASTA
    for the reference sequence.
  read_requirements: optional ReadRequirement proto. If not None, this proto
    is used to control which reads are filtered out by the reader before
    they are passed to the client.
  parse_aux_fields: optional bool, defaulting to False. If False, we do not
    parse the auxiliary fields of the SAM/BAM/CRAM records (see SAM spec for
    details). Parsing the aux fields is unnecessary for many applications,
    and adds a significant parsing cost to access. If you need these aux
    fields, set parse_aux_fields to True and these fields will be parsed and
    populate the appropriate Read proto fields (e.g., read.info).
  hts_block_size: int or None. If specified, this configures the block size
    of the underlying htslib file object. Larger values (e.g. 1M) may be
    beneficial for reading remote files. If None, the reader uses the
    default htslib block size.
  downsample_fraction: float in the interval [0.0, 1.0] or None. If
    specified as a positive float, the reader will only keep each read with
    probability downsample_fraction, randomly. If None or zero, all reads
    are kept.
  random_seed: None or int. The random seed to use with this sam reader, if
    needed. If None, a fixed random value will be assigned.
  use_original_base_quality_scores: optional bool, defaulting to False. If
    True, quality scores are read from OQ tag.

Raises:
  ValueError: If downsample_fraction is not None and not in the interval
    (0.0, 1.0].
  ImportError: If someone tries to load a tfbam file.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of Read protos in the file.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the reads in the region.
```

### NativeSamWriter
```
Class for writing to native SAM/BAM/CRAM files.

Most users will want SamWriter, which will write to either native SAM/BAM/CRAM
files or TFRecords files, based on the output filename's extensions.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, header, ref_path=None, embed_ref=False)`
```
Initializer for NativeSamWriter.

Args:
  output_path: str. A path where we'll write our SAM/BAM/CRAM file.
  ref_path: str. Path to the reference file. Required for CRAM file.
  embed_ref: bool. Whether to embed the reference sequences in CRAM file.
    Default is False.
  header: A nucleus.SamHeader proto.  The header is used both for writing
    the header, and to control the sorting applied to the rest of the file.
```

<a name="write"></a>
##### `write(self, proto)`


### SamReader
```
Class for reading Read protos from SAM/BAM/CRAM or TFRecord files.
```

### SamWriter
```
Class for writing Read protos to SAM or TFRecord files.
```

