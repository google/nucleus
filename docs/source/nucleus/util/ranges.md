# nucleus.util.ranges -- Utilities for Range overlap detection.
**Source code:** [nucleus/util/ranges.py](https://github.com/google/nucleus/tree/master/nucleus/util/ranges.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---


## Classes overview
Name | Description
-----|------------
[`RangeSet`](#rangeset) | Fast overlap detection of a genomic position against a database of Ranges.

## Functions overview
Name | Description
-----|------------
[`as_tuple`](#as_tuple)`(range_)` | Returns a Python tuple (reference_name, start, end).
[`bed_parser`](#bed_parser)`(filename)` | Parses Range objects from a BED-formatted file object.
[`bedpe_parser`](#bedpe_parser)`(filename)` | Parses Range objects from a BEDPE-formatted file object.
[`contigs_dict`](#contigs_dict)`(contigs)` | Creates a dictionary for contigs.
[`contigs_n_bases`](#contigs_n_bases)`(contigs)` | Returns the sum of all n_bases of contigs.
[`expand`](#expand)`(region, n_bp, contig_map=None)` | Expands region by n_bp in both directions.
[`find_max_overlapping`](#find_max_overlapping)`(query_range, search_ranges)` | Gets the index of the element in search_ranges with max overlap with query.
[`from_regions`](#from_regions)`(regions, contig_map=None)` | Parses each region of `regions` into a Range proto.
[`length`](#length)`(region)` | Returns the length in basepairs of region.
[`make_position`](#make_position)`(chrom, position, reverse_strand=False)` | Returns a nucleus.genomics.v1.Position.
[`make_range`](#make_range)`(chrom, start, end)` | Returns a nucleus.genomics.v1.Range.
[`overlap_len`](#overlap_len)`(range1, range2)` | Computes the number of overlapping bases of range1 and range2.
[`parse_literal`](#parse_literal)`(region_literal, contig_map=None)` | Parses a Range from a string representation like chr:start-end.
[`parse_literals`](#parse_literals)`(region_literals, contig_map=None)` | Parses each literal of region_literals in order.
[`position_overlaps`](#position_overlaps)`(chrom, pos, interval)` | Returns True iff the position chr:pos overlaps the interval.
[`ranges_overlap`](#ranges_overlap)`(i1, i2)` | Returns True iff ranges i1 and i2 overlap.
[`sorted_ranges`](#sorted_ranges)`(ranges, contigs=None)` | Sorts ranges by reference_name, start, and end.
[`span`](#span)`(regions)` | Returns a region that spans all of the bases in regions.
[`to_literal`](#to_literal)`(range_pb)` | Converts Range protobuf into string literal form.

## Classes
### RangeSet
```
Fast overlap detection of a genomic position against a database of Ranges.

Enables O(log n) computation of whether a point chr:pos falls within one of a
large number of genomic ranges.

This class does not supports overlapping or adjacent intervals. Any such
intervals will be automatically merged together in the constructor.

This class is immutable. No methods should be added that directly modify the
ranges held by the class.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, ranges=None, contigs=None)`
```
Creates a RangeSet backed by ranges.

Note that the Range objects in ranges are *not* stored directly here, so
they can safely be modified after they are passed to this RangeSet.

Args:
  ranges: list(nucleus.genomics.v1.Range) protos (or anything with
    reference_name, start, and end properties following the Range
    convention). If None, no ranges will be used, and overlaps() will always
    return False.
  contigs: list(nucleus.genomics.v1.ContigInfo) protos. Used to define the
    iteration order over contigs (i.e., by contig.pos_in_fasta).  If this
    list is not provided, the iteration order will be determined by the
    alphabetical order of the contig names.

Raises:
  ValueError: if any range's reference_name does not correspond to any
    contig in `contigs`.
```

<a name="exclude_regions"></a>
##### `exclude_regions(self, other)`
```
Chops out all of the intervals in other from this this RangeSet.

NOTE: This is a *MUTATING* operation for performance reasons. Make a copy
of self if you want to avoid modifying the RangeSet.

Args:
  other: A RangeSet object whose intervals will be removed from this
    RangeSet.
```

<a name="from_bed"></a>
##### `from_bed(cls, source, contigs=None)`
```
Creates a RangeSet containing the intervals from source.

Args:
  source: A path to a BED (or equivalent) file of intervals.
  contigs: An optional list of ContigInfo proto, used by RangeSet
    constructor.

Returns:
  A RangeSet.
```

<a name="from_contigs"></a>
##### `from_contigs(cls, contigs)`
```
Creates a RangeSet with an interval covering each base of each contig.
```

<a name="from_regions"></a>
##### `from_regions(cls, regions, contig_map=None)`
```
Parses a command-line style literal regions flag into a RangeSet.

Args:
  regions: An iterable or None. If not None, regions will be parsed with
    ranges.from_regions.
  contig_map: An optional dictionary mapping from contig names to ContigInfo
    protobufs. If provided, allows literals of the format "contig_name",
    which will be parsed into a Range with reference_name=contig_name,
    start=0, end=n_bases where n_bases comes from the ContigInfo;
    additionally the sort order of the RangeSet will be determined by
    contig.pos_in_fasta.

Returns:
  A RangeSet object.
```

<a name="intersection"></a>
##### `intersection(self, *others)`
```
Computes the intersection among this RangeSet and *others RangeSets.

This function computes the intersection of all of the intervals in self and
*others, returning a RangeSet containing only intervals common to all. The
intersection here is an ranged intersection, not an identity intersection,
so the resulting set of intervals may not contain any of the original
intervals in any of the sets.

To be concrete, suppose we have three sets to intersect, each having two
intervals:

  self   : chr1:1-10, chr2:20-30
  other1 : chr1:5-8, chr3:10-40
  other2 : chr1:3-7, chr3:10-30

self.intersection(other1, other2) produces a RangeSet with one interval
chr1:5-7, the common bases on chr1 in self, other1, and other2. No intervals
on chr2 or chr3 are included since the chr2 only occurs in self and the two
intervals on chr3, despite having some shared bases, don't have an
overlapping interval in self.

Args:
  *others: A list of RangeSet objects to intersect with the intervals in
    this RangeSet.

Returns:
  A RangeSet. If *others is empty, this function returns self rather than
  making an unnecessary copy. In all other cases, the returned value will be
  a freshly allocated RangeSet.
```

<a name="overlaps"></a>
##### `overlaps(self, chrom, pos)`
```
Returns True if chr:pos overlaps with any range in this RangeSet.

Uses a fast bisection algorithm to determine the overlap in O(log n) time.

Args:
  chrom: str. The chromosome name.
  pos: int. The position (0-based).

Returns:
  True if chr:pos overlaps with a range.
```

<a name="partition"></a>
##### `partition(self, max_size)`
```
Splits our intervals so that none are larger than max_size.

Slices up the intervals in this RangeSet into a equivalent set of intervals
(i.e., spanning the same set of bases), each of which is at most max_size in
length.

This function does not modify this RangeSet.

Because RangeSet merges adjacent intervals, this function cannot use a
RangeSet to represent the partitioned intervals and so instead generates
these intervals via a yield statement.

Args:
  max_size: int > 0. The maximum size of any interval.

Yields:
  nucleus.genomics.v1.Range protos, in sorted order (see comment about order
  in __iter__).

Raises:
  ValueError: if max_size <= 0.
```

<a name="variant_overlaps"></a>
##### `variant_overlaps(self, variant, empty_set_return_value=True)`
```
Returns True if the variant's range overlaps with any in this set.
```

## Functions
<a name="as_tuple"></a>
### `as_tuple(range_)`
```
Returns a Python tuple (reference_name, start, end).
```

<a name="bed_parser"></a>
### `bed_parser(filename)`
```
Parses Range objects from a BED-formatted file object.

See http://bedtools.readthedocs.org/en/latest/content/general-usage.html
for more information on the BED format.

Args:
  filename: file name of a BED-formatted file.

Yields:
  nucleus.genomics.v1.Range protobuf objects.
```

<a name="bedpe_parser"></a>
### `bedpe_parser(filename)`
```
Parses Range objects from a BEDPE-formatted file object.

See http://bedtools.readthedocs.org/en/latest/content/general-usage.html
for more information on the BEDPE format.

Skips events that span across chromosomes. For example, if the starting
location is on chr1 and the ending location is on chr2, that record will
not appear in the output.

Args:
  filename: file name of a BEDPE-formatted file.

Yields:
  nucleus.genomics.v1.Range protobuf objects.
```

<a name="contigs_dict"></a>
### `contigs_dict(contigs)`
```
Creates a dictionary for contigs.

Args:
  contigs: Iterable of ContigInfo protos.

Returns:
  A dictionary mapping contig.name: contig for each contig in contigs.
```

<a name="contigs_n_bases"></a>
### `contigs_n_bases(contigs)`
```
Returns the sum of all n_bases of contigs.
```

<a name="expand"></a>
### `expand(region, n_bp, contig_map=None)`
```
Expands region by n_bp in both directions.

Takes a Range(chrom, start, stop) and returns a new
Range(chrom, new_start, new_stop), where:

-- new_start is max(start - n_bp, 0)
-- new_stop is stop + n_bp if contig_map is None, or min(stop + n_bp, max_bp)
   where max_bp is contig_map[chrom].n_bp.

Args:
  region: A nucleus.genomics.v1.Range proto.
  n_bp: int >= 0. how many basepairs to increase region by.
  contig_map: dict[string, ContigInfo] or None. If not None, used to get the
    maximum extent to increase stop by. Must have region.reference_name as a
    key.

Returns:
  nucleus.genomics.v1.Range proto.

Raises:
  ValueError: if n_bp is invalid.
  KeyError: contig_map is not None and region.reference_name isn't a key.
```

<a name="find_max_overlapping"></a>
### `find_max_overlapping(query_range, search_ranges)`
```
Gets the index of the element in search_ranges with max overlap with query.

In case of ties, selects the lowest index range in search_ranges.

Args:
  query_range: nucleus.genomics.v1.Range, read genomic range.
  search_ranges: list[nucleus.genomics.v1.Read]. The list of regions we want
    to search for the maximal overlap with query_range. NOTE: this must be a
    list (not a generator) as we loop over the search_ranges multiple times.

Returns:
  int, the search_ranges index with the maximum read overlap. Returns None
  when read has no overlap with any of the search_ranges or search_ranges is
  empty.
```

<a name="from_regions"></a>
### `from_regions(regions, contig_map=None)`
```
Parses each region of `regions` into a Range proto.

This function provides a super high-level interface for
reading/parsing/converting objects into Range protos. Each `region` of
`regions` is processed in turn, yielding one or more Range protos. This
function inspects the contents of `region` to determine how to convert it to
Range(s) protos. The following types of `region` strings are supported:

  * If region ends with an extension known in _get_parser_for_file, we treat
    region as a file and read the Range protos from it with the corresponding
    reader from _get_parser_for_file, yielding each Range from the file in
    order.
  * Otherwise we parse region as a region literal (`chr20:1-10`) and return
    the Range proto.

Args:
  regions: iterable[str]. Converts each element of this iterable into
    region(s).
  contig_map: An optional dictionary mapping from contig names to ContigInfo
    protobufs. If provided, allows literals of the format "contig_name",
    which will be parsed into a Range with reference_name=contig_name,
    start=0, end=n_bases where n_bases comes from the ContigInfo.

Yields:
  A Range proto.
```

<a name="length"></a>
### `length(region)`
```
Returns the length in basepairs of region.
```

<a name="make_position"></a>
### `make_position(chrom, position, reverse_strand=False)`
```
Returns a nucleus.genomics.v1.Position.

Args:
  chrom: str. The chromosome name.
  position: int. The start position (0-based, inclusive).
  reverse_strand: bool. If True, indicates the position is on the negative
    strand.
```

<a name="make_range"></a>
### `make_range(chrom, start, end)`
```
Returns a nucleus.genomics.v1.Range.

Args:
  chrom: str. The chromosome name.
  start: int. The start position (0-based, inclusive) of this range.
  end: int. The end position (0-based, exclusive) of this range.

Returns:
  A nucleus.genomics.v1.Range.
```

<a name="overlap_len"></a>
### `overlap_len(range1, range2)`
```
Computes the number of overlapping bases of range1 and range2.

Args:
  range1: nucleus.genomics.v1.Range.
  range2: nucleus.genomics.v1.Range.

Returns:
  int. The number of basepairs in common. 0 if the ranges are not on the same
  contig.
```

<a name="parse_literal"></a>
### `parse_literal(region_literal, contig_map=None)`
```
Parses a Range from a string representation like chr:start-end.

The region literal must conform to the following pattern:

  chromosome:start-end
  chromosome:position
  chromosome  [if contig_map is provided]

chromosome can be any non-empty string without whitespace. start and end must
both be positive integers. They can contain commas for readability. start and
end are positions not offsets, so start == 1 means an offset of zero. If only
a single position is provided, this creates a 1 bp interval starting at
position - 1 and ending at position.

Inspired by the samtools region specification:
http://www.htslib.org/doc/samtools.html

Args:
  region_literal: str. The literal to parse.
  contig_map: An optional dictionary mapping from contig names to ContigInfo
    protobufs. If provided, allows literals of the format "contig_name", which
    will be parsed into a Range with reference_name=contig_name, start=0,
    end=n_bases where n_bases comes from the ContigInfo.

Returns:
  nucleus.genomics.v1.Range.

Raises:
  ValueError: if region_literal cannot be parsed.
```

<a name="parse_literals"></a>
### `parse_literals(region_literals, contig_map=None)`
```
Parses each literal of region_literals in order.
```

<a name="position_overlaps"></a>
### `position_overlaps(chrom, pos, interval)`
```
Returns True iff the position chr:pos overlaps the interval.

Args:
  chrom: str. The chromosome name.
  pos: int. The position (0-based, inclusive).
  interval: nucleus.genomics.v1.Range object.

Returns:
  True if interval overlaps chr:pos.
```

<a name="ranges_overlap"></a>
### `ranges_overlap(i1, i2)`
```
Returns True iff ranges i1 and i2 overlap.

Args:
  i1: nucleus.genomics.v1.Range object.
  i2: nucleus.genomics.v1.Range object.

Returns:
  True if and only if i1 and i2 overlap.
```

<a name="sorted_ranges"></a>
### `sorted_ranges(ranges, contigs=None)`
```
Sorts ranges by reference_name, start, and end.

Args:
  ranges: Iterable of nucleus.genomics.v1.Range protos that we want to sort.
  contigs: None or an iterable of ContigInfo protos. If not None, we will use
    the order of the contigs (as defined by their pos_in_fasta field values)
    to sort the Ranges on different contigs with respect to each other.

Returns:
  A newly allocated list of nucleus.genomics.v1.Range protos.
```

<a name="span"></a>
### `span(regions)`
```
Returns a region that spans all of the bases in regions.

This function returns a Range(chrom, start, stop), where start is the min
of the starts in regions, and stop is the max end in regions. It may not be
freshly allocated.

Args:
  regions: list[Range]: a list of Range protos.

Returns:
  A single Range proto.

Raises:
  ValueError: if not all regions have the same reference_name.
  ValueError: if regions is empty.
```

<a name="to_literal"></a>
### `to_literal(range_pb)`
```
Converts Range protobuf into string literal form.

The string literal form looks like:

  reference_name:start+1-end

since start and end are zero-based inclusive (start) and exclusive (end),
while the literal form is one-based inclusive on both ends.

Args:
  range_pb: A nucleus.genomics.v1.Range object.

Returns:
  A string representation of the Range.
```

