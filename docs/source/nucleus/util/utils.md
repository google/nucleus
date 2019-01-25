# nucleus.util.utils -- Utility functions for working with reads.
**Source code:** [nucleus/util/utils.py](https://github.com/google/nucleus/tree/master/nucleus/util/utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---


## Functions overview
Name | Description
-----|------------
[`read_overlaps_region`](#read_overlaps_region)`(read, region)` | Returns True if read overlaps read.
[`read_range`](#read_range)`(read)` | Creates a Range proto from the alignment of Read.
[`reservoir_sample`](#reservoir_sample)`(iterable, k, random=None)` | Samples k elements with uniform probability from an iterable.

## Functions
<a name="read_overlaps_region"></a>
### `read_overlaps_region(read, region)`
```
Returns True if read overlaps read.

This function is equivalent to calling:

  `ranges.ranges_overlap(region, read_range(read))`

But is optimized for speed and memory performance in C++.

Args:
  read: nucleus.genomics.v1.Read.
  region: nucleus.genomics.v1.Range.

Returns:
  True if read and region overlap (i.e, have the same reference_name and their
  start/ends overlap at least one basepair).
```

<a name="read_range"></a>
### `read_range(read)`
```
Creates a Range proto from the alignment of Read.

Args:
  read: nucleus.genomics.v1.Read. The read to calculate the range for.

Returns:
  A nucleus.genomics.v1.Range for read.
```

<a name="reservoir_sample"></a>
### `reservoir_sample(iterable, k, random=None)`
```
Samples k elements with uniform probability from an iterable.

Selects a subset of k elements from n input elements with uniform probability
without needing to hold all n elements in memory at the same time. This
implementation has max space complexity O(min(k, n)), i.e., we allocate up to
min(k, n) elements to store the samples. This means that we only use ~n
elements when n is smaller than k, which can be important when k is large. If
n elements are added to this sampler, and n <= k, all n elements will be
retained. If n > k, each added element will be retained with a uniform
probability of k / n.

The order of the k retained samples from our n elements is undefined. In
particular that means that the elements in the returned list can occur in a
different order than they appeared in the iterable.

More details about reservoir sampling (and the specific algorithm used here
called Algorithm R) can be found on wikipedia:

https://en.wikipedia.org/wiki/Reservoir_sampling#Algorithm_R

Args:
  iterable: Python iterable. The iterable to sample from.
  k: int. The number of elements to sample.
  random: A random number generator or None.

Returns:
  A list containing the k sampled elements.

Raises:
  ValueError: If k is negative.
```

