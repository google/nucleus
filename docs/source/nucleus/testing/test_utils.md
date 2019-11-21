# nucleus.testing.test_utils -- Utilities to help with testing code.
**Source code:** [nucleus/testing/test_utils.py](https://github.com/google/nucleus/tree/master/nucleus/testing/test_utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---


## Functions overview
Name | Description
-----|------------
[`assert_called_once_workaround`](#assert_called_once_workaround)`(mock)` | Asserts that a mock has been called exactly once.
[`assert_not_called_workaround`](#assert_not_called_workaround)`(mock)` | Asserts that a mock has not been called.
[`cc_iterable_len`](#cc_iterable_len)`(cc_iterable)` | Count the number of elements in an Iterable object.
[`genomics_core_testdata`](#genomics_core_testdata)`(filename)` | Gets the path to a testdata named filename in util/testdata.
[`genomics_testdata`](#genomics_testdata)`(path, datadir=DATADIR)` | Gets the path to a testdata file in genomics at relative path.
[`iterable_len`](#iterable_len)`(iterable)` | Returns the length of a Python iterable, by advancing it.
[`make_read`](#make_read)`(bases, start, quals=None, cigar=None, mapq=50, chrom='chr1', name=None)` | Makes a nucleus.genomics.v1.Read for testing.
[`make_variant`](#make_variant)`(chrom='chr1', start=10, alleles=None, end=None, filters=None, qual=None, gt=None, gq=None, sample_name=None, gls=None, is_phased=None)` | Creates a new Variant proto from args.
[`make_variant_multiple_calls`](#make_variant_multiple_calls)`(chrom='chr1', start=10, alleles=None, end=None, filters=None, qual=None, gts=None, gqs=None, sample_names=None, glss=None, is_phased=None)` | Creates a new Variant proto from args that contains multi-sample calls.
[`set_list_values`](#set_list_values)`(list_value, values)` | Sets a ListValue to have the values in values.
[`test_tmpfile`](#test_tmpfile)`(name, contents=None)` | Returns a path to a tempfile named name in the test_tmpdir.

## Functions
<a name="assert_called_once_workaround"></a>
### `assert_called_once_workaround(mock)`
```
Asserts that a mock has been called exactly once.

See assert_not_called_workaround for the backstory on why this function
exists.

Args:
  mock: The mock that should have been called exactly once.

Raises:
  AssertionError: mock wasn't called exactly once.
```

<a name="assert_not_called_workaround"></a>
### `assert_not_called_workaround(mock)`
```
Asserts that a mock has not been called.

There's a bug in mock where some of the assert functions on a mock are being
dropped when that mock is created with an autospec:

  https://bugs.python.org/issue28380

The mock 2.0.0 backport doesn't have the fix yet. The required patch is:

  https://bugs.python.org/file44991/fix_autospecced_mock_functions.patch

but the current mock (checked 07/22/17) backport code is missing the fix:

  https://github.com/testing-cabal/mock/blob/master/mock/mock.py#L315

This is an open issue on the mock github repo:

  https://github.com/testing-cabal/mock/issues/398

And they claim that it'll be a few months (as of April 2017) before it is
incorporated into the backport.

Args:
  mock: The mock to assert hasn't been called.

Raises:
  AssertionError: mock has been called.
```

<a name="cc_iterable_len"></a>
### `cc_iterable_len(cc_iterable)`
```
Count the number of elements in an Iterable object.

Args:
  cc_iterable: a CLIF-wrap of a subclass of the C++ Iterable class.

Returns:
  integer count
```

<a name="genomics_core_testdata"></a>
### `genomics_core_testdata(filename)`
```
Gets the path to a testdata named filename in util/testdata.

Args:
  filename: The name of a testdata file in the core genomics testdata
    directory. For example, if you have a test file in
    "third_party/nucleus/util/testdata/foo.txt", filename should be
    "foo.txt" to get a path to it.

Returns:
  The absolute path to a testdata file.
```

<a name="genomics_testdata"></a>
### `genomics_testdata(path, datadir=DATADIR)`
```
Gets the path to a testdata file in genomics at relative path.

Args:
  path: A path to a testdata file *relative* to the genomics root
    directory. For example, if you have a test file in
    "datadir/nucleus/testdata/foo.txt", path should be
    "nucleus/testdata/foo.txt" to get a path to it.
  datadir: The path of the genomics root directory *relative* to
    the testing source directory.

Returns:
  The absolute path to a testdata file.
```

<a name="iterable_len"></a>
### `iterable_len(iterable)`
```
Returns the length of a Python iterable, by advancing it.
```

<a name="make_read"></a>
### `make_read(bases, start, quals=None, cigar=None, mapq=50, chrom='chr1', name=None)`
```
Makes a nucleus.genomics.v1.Read for testing.
```

<a name="make_variant"></a>
### `make_variant(chrom='chr1', start=10, alleles=None, end=None, filters=None, qual=None, gt=None, gq=None, sample_name=None, gls=None, is_phased=None)`
```
Creates a new Variant proto from args.

Args:
  chrom: str. The reference_name for this variant.
  start: int. The starting position of this variant.
  alleles: list of str with at least one element. alleles[0] is the reference
    bases and alleles[1:] will be set to alternate_bases of variant. If None,
    defaults to ['A', 'C'].
  end: int or None. If not None, the variant's end will be set to this value.
    If None, will be set to the start + len(reference_bases).
  filters: str, list of str, or None. Sets the filters field of the variant to
    this value if not None. If filters is a string `value`, this is equivalent
    to an argument [`value`]. If None, no value will be assigned to the
    filters field.
  qual: int or None. The quality score for this variant. If None, no quality
    score will be written in the Variant.
  gt: A list of ints, or None. If present, creates a VariantCall in Variant
    with genotype field set to this value. The special 'DEFAULT' value, if
    provided, will set the genotype to [0, 1]. This is the default behavior.
  gq: int or None. If not None and gt is not None, we will add an this GQ
    value to our VariantCall.
  sample_name: str or None. If not None and gt is not None, sets the
    call_set_name of our VariantCall to this value.
  gls: array-list of float, or None. If not None and gt is not None, sets the
    genotype_likelihoods of our VariantCall to this value.
  is_phased: bool. Indicates whether a VariantCall should be phased.

Returns:
  nucleus.genomics.v1.Variant proto.
```

<a name="make_variant_multiple_calls"></a>
### `make_variant_multiple_calls(chrom='chr1', start=10, alleles=None, end=None, filters=None, qual=None, gts=None, gqs=None, sample_names=None, glss=None, is_phased=None)`
```
Creates a new Variant proto from args that contains multi-sample calls.

Args:
  chrom: str. The reference_name for this variant.
  start: int. The starting position of this variant.
  alleles: list of str with at least one element. alleles[0] is the reference
    bases and alleles[1:] will be set to alternate_bases of variant. If None,
      defaults to ['A', 'C'].
  end: int or None. If not None, the variant's end will be set to this value.
    If None, will be set to the start + len(reference_bases).
  filters: str, list of str, or None. Sets the filters field of the variant to
    this value if not None. If filters is a string `value`, this is equivalent
    to an argument [`value`]. If None, no value will be assigned to the
    filters field.
  qual: int or None. The quality score for this variant. If None, no quality
    score will be written in the Variant.
  gts: A list of lists of ints. For each list in this list, creates a
    VariantCall in Variant with genotype field set to this value.
  gqs: A list of ints or None. Must match the gts arg if specified. Sets the
    GQ value of corresponding VariantCall.
  sample_names: A list of strs or None. Must match the gts arg if specified.
    Sets the call_set_name of the corresponding VariantCall.
  glss: A list of array-lists of float, or None. Must match the gts arg if
    specified. Sets the genotype_likelihoods of the corresponding VariantCall.
  is_phased: list of bools. Must match the gts arg if specified. Indicates
    whether the corresponding VariantCall should be phased.

Returns:
  nucleus.genomics.v1.Variant proto.
```

<a name="set_list_values"></a>
### `set_list_values(list_value, values)`
```
Sets a ListValue to have the values in values.
```

<a name="test_tmpfile"></a>
### `test_tmpfile(name, contents=None)`
```
Returns a path to a tempfile named name in the test_tmpdir.

Args:
  name: str; the name of the file, should not contain any slashes.
  contents: bytes, or None. If not None, tmpfile's contents will be set to
    contents before returning the path.

Returns:
  str path to a tmpfile with filename name in our test tmpfile directory.
```

