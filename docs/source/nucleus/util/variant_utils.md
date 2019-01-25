# nucleus.util.variant_utils -- Variant utilities.
**Source code:** [nucleus/util/variant_utils.py](https://github.com/google/nucleus/tree/master/nucleus/util/variant_utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---


## Classes overview
Name | Description
-----|------------
[`AlleleMismatchType`](#allelemismatchtype) | An enumeration of the types of allele mismatches we detect.
[`GenotypeType`](#genotypetype) | An enumeration of the types of genotypes.
[`VariantType`](#varianttype) | An enumeration of the types of variants.

## Functions overview
Name | Description
-----|------------
[`allele_indices_for_genotype_likelihood_index`](#allele_indices_for_genotype_likelihood_index)`(gl_index, ploidy=2)` | Returns a tuple of allele_indices corresponding to the given GL index.
[`allele_indices_with_num_alts`](#allele_indices_with_num_alts)`(variant, num_alts, ploidy=2)` | Returns a list of allele indices configurations with `num_alts` alternates.
[`allele_mismatches`](#allele_mismatches)`(evalv, truev)` | Determines the set of allele mismatch discordances between evalv and truev.
[`calc_ac`](#calc_ac)`(variant)` | Returns a list of alt counts based on variant.calls.
[`calc_an`](#calc_an)`(variant)` | Returns the total number of alleles in called genotypes in variant.
[`decode_variants`](#decode_variants)`(encoded_iter)` | Yields a genomics.Variant from encoded_iter.
[`format_alleles`](#format_alleles)`(variant)` | Gets a string representation of the variant's alleles.
[`format_filters`](#format_filters)`(variant)` | Returns a human-readable string showing the filters applied to variant.
[`format_position`](#format_position)`(variant)` | Gets a string representation of the variant's position.
[`genotype_as_alleles`](#genotype_as_alleles)`(variant, call_ix=0)` | Gets genotype of the sample in variant as a list of actual alleles.
[`genotype_likelihood`](#genotype_likelihood)`(variant_call, allele_indices)` | Returns the genotype likelihood for the given allele indices.
[`genotype_likelihood_index`](#genotype_likelihood_index)`(allele_indices)` | Returns the genotype likelihood index for the given allele indices.
[`genotype_ordering_in_likelihoods`](#genotype_ordering_in_likelihoods)`(variant)` | Yields (i, j, allele_i, allele_j) for the genotypes ordering in GLs.
[`genotype_type`](#genotype_type)`(variant)` | Gets the GenotypeType for variant.
[`get_info`](#get_info)`(variant, field_name, vcf_object=None)` | Returns the value of the `field_name` INFO field.
[`has_calls`](#has_calls)`(variant)` | Does variant have any genotype calls?
[`has_deletion`](#has_deletion)`(variant)` | Does variant have a deletion?
[`has_insertion`](#has_insertion)`(variant)` | Does variant have an insertion?
[`is_biallelic`](#is_biallelic)`(variant)` | Returns True if variant has exactly one alternate allele.
[`is_deletion`](#is_deletion)`(ref, alt)` | Is alt a deletion w.r.t. ref?
[`is_filtered`](#is_filtered)`(variant)` | Returns True if variant has a non-PASS filter field, or False otherwise.
[`is_gvcf`](#is_gvcf)`(variant)` | Returns true if variant encodes a standard gVCF reference block.
[`is_indel`](#is_indel)`(variant, exclude_alleles=None)` | Is variant an indel?
[`is_insertion`](#is_insertion)`(ref, alt)` | Is alt an insertion w.r.t. ref?
[`is_multiallelic`](#is_multiallelic)`(variant)` | Does variant have multiple alt alleles?
[`is_ref`](#is_ref)`(variant)` | Returns true if variant is a reference record.
[`is_snp`](#is_snp)`(variant, exclude_alleles=None)` | Is variant a SNP?
[`is_transition`](#is_transition)`(allele1, allele2)` | Is the pair of single bp alleles a transition?
[`is_variant_call`](#is_variant_call)`(variant, require_non_ref_genotype=True, no_calls_are_variant=False, call_indices=None)` | Is variant a non-reference call?
[`only_call`](#only_call)`(variant)` | Ensures the Variant has exactly one VariantCall, and returns it.
[`set_info`](#set_info)`(variant, field_name, value, vcf_object=None)` | Sets a field of the info map of the `Variant` to the given value(s).
[`simplify_alleles`](#simplify_alleles)`(*alleles)` | Simplifies alleles by stripping off common postfix bases.
[`sorted_variants`](#sorted_variants)`(variants)` | Returns sorted(variants, key=variant_range_tuple).
[`variant_key`](#variant_key)`(variant, sort_alleles=True)` | Gets a human-readable string key that is almost unique for Variant.
[`variant_position`](#variant_position)`(variant)` | Returns a new Range at the start position of variant.
[`variant_range`](#variant_range)`(variant)` | Returns a new Range covering variant.
[`variant_range_tuple`](#variant_range_tuple)`(variant)` | Returns a new tuple of (reference_name, start, end) for the variant.
[`variant_type`](#variant_type)`(variant)` | Gets the VariantType of variant.
[`variants_are_sorted`](#variants_are_sorted)`(variants)` | Returns True if variants are sorted w.r.t. variant_range.
[`variants_overlap`](#variants_overlap)`(variant1, variant2)` | Returns True if the range of variant1 and variant2 overlap.

## Classes
### AlleleMismatchType
```
An enumeration of the types of allele mismatches we detect.
```

### GenotypeType
```
An enumeration of the types of genotypes.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, full_name, example_gt, class_id)`
```
Create a GenotypeType with the given name, GT and class_id.
```

### VariantType
```
An enumeration of the types of variants.
```

## Functions
<a name="allele_indices_for_genotype_likelihood_index"></a>
### `allele_indices_for_genotype_likelihood_index(gl_index, ploidy=2)`
```
Returns a tuple of allele_indices corresponding to the given GL index.

This is the inverse function to `genotype_likelihood_index`.

Args:
  gl_index: int. The index within a genotype likelihood array for which to
    determine the associated alleles.
  ploidy: int. The ploidy of the result.

Returns:
  A tuple of `ploidy` ints representing the allele indices at this GL index.

Raises:
  NotImplementedError: The requested allele indices are more than diploid.
```

<a name="allele_indices_with_num_alts"></a>
### `allele_indices_with_num_alts(variant, num_alts, ploidy=2)`
```
Returns a list of allele indices configurations with `num_alts` alternates.

Args:
  variant: nucleus.genomics.v1.Variant. The variant of interest, which
    defines the candidate alternate alleles that can be used to generate
    allele indices configurations.
  num_alts: int in [0, `ploidy`]. The number of non-reference alleles for
    which to create the allele indices configurations.
  ploidy: int. The ploidy for which to return allele indices configurations.

Returns: A list of tuples. Each tuple is of length `ploidy` and represents the
  allele indices of all `ploidy` genotypes that contain `num_alts`
  non-reference alleles.

Raises:
  ValueError: The domain of `num_alts` is invalid.
  NotImplementedError: `ploidy` is not diploid.
```

<a name="allele_mismatches"></a>
### `allele_mismatches(evalv, truev)`
```
Determines the set of allele mismatch discordances between evalv and truev.

Compares the alleles present in evalv and truev to determine if there are any
disagreements between the set of called alleles in the two Variant protos. The
type of differences basically boil down to:

-- Are there duplicate alt alleles?
-- Can we find a matching allele in the truev for each allele in evalv, and
  vice versa?

Two alleles A and B match when they would produce the same sequence of bases
in ref and alt haplotypes starting at the same position. So CA=>TA is the same
as C=>T (position is the same, replacing A by A is a noop) but AC=>AT isn't
the same as C=>T because the former event changes bases 1 bp further along in
the reference genome than the C=>T allele.

Args:
  evalv: A nucleus.genomics.v1.Variant.
  truev: A nucleus.genomics.v1.Variant.

Returns:
  A set of AlleleMismatchType values.
```

<a name="calc_ac"></a>
### `calc_ac(variant)`
```
Returns a list of alt counts based on variant.calls.
```

<a name="calc_an"></a>
### `calc_an(variant)`
```
Returns the total number of alleles in called genotypes in variant.
```

<a name="decode_variants"></a>
### `decode_variants(encoded_iter)`
```
Yields a genomics.Variant from encoded_iter.

Args:
  encoded_iter: An iterable that produces binary encoded
    nucleus.genomics.v1.Variant strings.

Yields:
  A parsed nucleus.genomics.v1.Variant for each encoded element of
  encoded_iter in order.
```

<a name="format_alleles"></a>
### `format_alleles(variant)`
```
Gets a string representation of the variant's alleles.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A string ref_bases/alt1,alt2 etc.
```

<a name="format_filters"></a>
### `format_filters(variant)`
```
Returns a human-readable string showing the filters applied to variant.

Returns a string with the filter field values of variant separated by commas.
If the filter field isn't set, returns vcf_constants.MISSING_FIELD ('.').

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A string.
```

<a name="format_position"></a>
### `format_position(variant)`
```
Gets a string representation of the variant's position.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A string chr:start + 1 (as start is zero-based).
```

<a name="genotype_as_alleles"></a>
### `genotype_as_alleles(variant, call_ix=0)`
```
Gets genotype of the sample in variant as a list of actual alleles.

Returns the alleles specified by the genotype indices of variant.calls[0].
For example, if variant.reference_bases = 'A' and variant.alternative_bases
= ['C'] and the genotypes are [0, 1], this function will return
['A', 'C'].

Args:
  variant: nucleus.genomics.v1.Variant.
  call_ix: int. The index into the calls attribute indicating which
    VariantCall to return alleles for.

Returns:
  A list of allele (string) from variant, one for each genotype in
  variant.calls[call_ix], in order.

Raises:
  ValueError: If variant doesn't have a call at the specified index.
```

<a name="genotype_likelihood"></a>
### `genotype_likelihood(variant_call, allele_indices)`
```
Returns the genotype likelihood for the given allele indices.

Args:
  variant_call: nucleus.genomics.v1.VariantCall. The VariantCall from
    which to extract the genotype likelihood of the allele indices.
  allele_indices: list(int). The list of allele indices for a given genotype.
    E.g. diploid heterozygous alternate can be represented as [0, 1].

Returns:
  The float value of the genotype likelihood of this set of alleles.
```

<a name="genotype_likelihood_index"></a>
### `genotype_likelihood_index(allele_indices)`
```
Returns the genotype likelihood index for the given allele indices.

Args:
  allele_indices: list(int). The list of allele indices for a given genotype.
    E.g. diploid homozygous reference is represented as [0, 0].

Returns:
  The index into the associated genotype likelihood array corresponding to
  the likelihood of this list of alleles.

Raises:
  NotImplementedError: The allele_indices are more than diploid.
```

<a name="genotype_ordering_in_likelihoods"></a>
### `genotype_ordering_in_likelihoods(variant)`
```
Yields (i, j, allele_i, allele_j) for the genotypes ordering in GLs.

https://samtools.github.io/hts-specs/VCFv4.1.pdf
"If A is the allele in REF and B,C,... are the alleles as ordered in ALT,
the ordering of genotypes for the likelihoods is given by:
F(j/k) = (k*(k+1)/2)+j. In other words, for biallelic sites the ordering is:
AA,AB,BB; for triallelic sites the ordering is: AA,AB,BB,AC,BC,CC, etc."
The biallelic sites in our case are 0/0, 0/1, 1/1.
The triallelic sites are 0/0, 0/1, 1/1, 0/2, 1/2, 2/2.
This wiki page has more information that generalizes ot different ploidy.
http://genome.sph.umich.edu/wiki/Relationship_between_Ploidy,_Alleles_and_Genotypes

Currently this function only implements for diploid cases.

Args:
  variant: nucleus.genomics.v1.Variant.

Yields:
  allele indices and strings (i, j, allele_i, allele_j) in the correct order.
```

<a name="genotype_type"></a>
### `genotype_type(variant)`
```
Gets the GenotypeType for variant.

If variant doesn't have genotypes, returns no_call. Otherwise
returns one of no_call, hom_ref, het, or hom_var depending on the
status of the genotypes in the call field of variant.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A GenotypeType.

Raises:
  ValueError: If variant has more than one call (i.e., is multi-sample).
```

<a name="get_info"></a>
### `get_info(variant, field_name, vcf_object=None)`
```
Returns the value of the `field_name` INFO field.

The `vcf_object` is used to determine the type of the resulting value. If it
is a single value or a Flag, that single value will be returned. Otherwise,
the list of values is returned.

Args:
  variant: Variant proto. The Variant of interest.
  field_name: str. The name of the field to retrieve values from.
  vcf_object: (Optional) A VcfReader or VcfWriter object. If not None, the
    type of the field is inferred from the associated VcfReader or VcfWriter
    based on its name. Otherwise, the type is inferred if it is a reserved
    field.
```

<a name="has_calls"></a>
### `has_calls(variant)`
```
Does variant have any genotype calls?

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  True if variant has one or more VariantCalls.
```

<a name="has_deletion"></a>
### `has_deletion(variant)`
```
Does variant have a deletion?

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  True if the alleles in variant indicate an deletion event
  occurs at this site.
```

<a name="has_insertion"></a>
### `has_insertion(variant)`
```
Does variant have an insertion?

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  True if the alleles in variant indicate an insertion event
  occurs at this site.
```

<a name="is_biallelic"></a>
### `is_biallelic(variant)`
```
Returns True if variant has exactly one alternate allele.
```

<a name="is_deletion"></a>
### `is_deletion(ref, alt)`
```
Is alt a deletion w.r.t. ref?

Args:
  ref: A string of the reference allele.
  alt: A string of the alternative allele.

Returns:
  True if alt is a deletion w.r.t. ref.
```

<a name="is_filtered"></a>
### `is_filtered(variant)`
```
Returns True if variant has a non-PASS filter field, or False otherwise.
```

<a name="is_gvcf"></a>
### `is_gvcf(variant)`
```
Returns true if variant encodes a standard gVCF reference block.

This means in practice that variant has a single alternate allele that is the
canonical gVCF allele vcf_constants.GVCF_ALT_ALLELE.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  Boolean. True if variant is a gVCF record, False otherwise.
```

<a name="is_indel"></a>
### `is_indel(variant, exclude_alleles=None)`
```
Is variant an indel?

An indel event is simply one where the size of at least one of the alleles
is > 1.

Args:
  variant: nucleus.genomics.v1.Variant.
  exclude_alleles: list(str). The alleles in this list will be ignored.

Returns:
  True if the alleles in variant indicate an insertion/deletion event
  occurs at this site.
```

<a name="is_insertion"></a>
### `is_insertion(ref, alt)`
```
Is alt an insertion w.r.t. ref?

Args:
  ref: A string of the reference allele.
  alt: A string of the alternative allele.

Returns:
  True if alt is an insertion w.r.t. ref.
```

<a name="is_multiallelic"></a>
### `is_multiallelic(variant)`
```
Does variant have multiple alt alleles?

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  True if variant has more than one alt allele.
```

<a name="is_ref"></a>
### `is_ref(variant)`
```
Returns true if variant is a reference record.

Variant protos can encode sites that aren't actually mutations in the
sample. For example, the record ref='A', alt='.' indicates that there is
no mutation present (i.e., alt is the missing value).

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A boolean.
```

<a name="is_snp"></a>
### `is_snp(variant, exclude_alleles=None)`
```
Is variant a SNP?

Args:
  variant: nucleus.genomics.v1.Variant.
  exclude_alleles: list(str). The alleles in this list will be ignored.

Returns:
  True if all alleles of variant are 1 bp in length, excluding the GVCF
  <*> allele.
```

<a name="is_transition"></a>
### `is_transition(allele1, allele2)`
```
Is the pair of single bp alleles a transition?

Args:
  allele1: A string of the first allele, must be 1 bp in length.
  allele2: A string of the second allele, must be 1 bp in length.

Returns:
  True if allele1/allele2 are a transition SNP.

Raises:
  ValueError: if allele1 and allele2 are equal or aren't 1 bp in length.
```

<a name="is_variant_call"></a>
### `is_variant_call(variant, require_non_ref_genotype=True, no_calls_are_variant=False, call_indices=None)`
```
Is variant a non-reference call?

A Variant proto doesn't always imply that there's a variant present in the
genome. The call may not have alternate bases, may be filtered, may a have
hom-ref genotype, etc. This function looks for all of those configurations
and returns true iff the variant is asserting that a mutation is present
in the same.

Note that this code allows a variant without a calls field to be variant,
but one with a genotype call must have a non-reference genotype to be
considered variant (if require_non_ref_genotype is True, the default). If
False, a variant that passes all of the site-level requirements for being
a variant_call will return a True value, regardless of the genotypes, which
means that we'll consider a site with a sample with a hom-ref or no-call site
a variant call.

Args:
  variant: nucleus.genomics.v1.Variant.
  require_non_ref_genotype: Should we require a site with a genotype call to
    have a non-reference (het, hom-var) genotype for the site to be considered
    a variant call?
  no_calls_are_variant: If a site has genotypes, should we consider no_call
    genotypes as being variant or not?
  call_indices: A list of 0-based indices. If specified, only the calls
    at the given indices will be considered. The function will return
    True if any of those calls are variant.

Returns:
  True if variant is really a mutation call.
```

<a name="only_call"></a>
### `only_call(variant)`
```
Ensures the Variant has exactly one VariantCall, and returns it.

Args:
  variant: nucleus.genomics.v1.Variant. The variant of interest.

Returns:
  The single nucleus.genomics.v1.VariantCall in the variant.

Raises:
  ValueError: Not exactly one VariantCall is in the variant.
```

<a name="set_info"></a>
### `set_info(variant, field_name, value, vcf_object=None)`
```
Sets a field of the info map of the `Variant` to the given value(s).

`variant.info` is analogous to the INFO field of a VCF record.

Args:
  variant: Variant proto. The Variant to modify.
  field_name: str. The name of the field to set.
  value: A single value or list of values to update the Variant with. The type
    of the value is determined by the `vcf_object` if one is given, otherwise
    is looked up based on the reserved INFO fields in the VCF specification.
  vcf_object: (Optional) A VcfReader or VcfWriter object. If not None, the
    type of the field is inferred from the associated VcfReader or VcfWriter
    based on its name. Otherwise, the type is inferred if it is a reserved
    field.
```

<a name="simplify_alleles"></a>
### `simplify_alleles(*alleles)`
```
Simplifies alleles by stripping off common postfix bases.

For example, simplify("AC", "GC") would produce the tuple "A", "G" as the "C"
base is a common postfix of both alleles. But simplify("AC", "GT") would
produce "AC", "GT" as there is no common postfix.

Note this function will never simplify any allele down to the empty string. So
if alleles = ['CACA', 'CA'], the longest common postfix is 'CA' but we will
not produce ['CA', ''] as this is an invalid Variant allele encoding. Instead
we produce ['CAC', 'C'].

Args:
  *alleles: A tuple of bases, each as a string, to simplify.

Returns:
  A tuple, one for each allele in alleles in order, with any common postfix
  bases stripped off.
```

<a name="sorted_variants"></a>
### `sorted_variants(variants)`
```
Returns sorted(variants, key=variant_range_tuple).
```

<a name="variant_key"></a>
### `variant_key(variant, sort_alleles=True)`
```
Gets a human-readable string key that is almost unique for Variant.

Gets a string key that contains key information about the variant, formatted
as:

  reference_name:start+1:reference_bases->alternative_bases

where alternative bases is joined with a '/' for each entry in
alternative_bases. The start+1 is so we display the position, which starts at
1, and not the offset, which starts at 0.

For example, a Variant(reference_name='20', start=10, reference_bases='AC',
alternative_bases=['A', 'ACC']) would have a key of:

  20:11:AC->A/ACC

The key is 'almost unique' in that the reference_name + start + alleles should
generally occur once within a single VCF file, given the way the VCF
specification works.

Args:
  variant: nucleus.genomics.v1.Variant to make into a key.
  sort_alleles: bool. If True, the alternative_bases of variant will be sorted
    according to their lexicographic order. If False, the alternative_bases
    will be displayed in their order in the Variant.

Returns:
  A str.
```

<a name="variant_position"></a>
### `variant_position(variant)`
```
Returns a new Range at the start position of variant.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A new Range with the same reference_name as variant and start but an end
  that is start + 1. This produces a range that is the single basepair of the
  start of variant, hence the name position.
```

<a name="variant_range"></a>
### `variant_range(variant)`
```
Returns a new Range covering variant.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A new Range with the same reference_name, start, and end as variant.
```

<a name="variant_range_tuple"></a>
### `variant_range_tuple(variant)`
```
Returns a new tuple of (reference_name, start, end) for the variant.

A common use case for this function is to sort variants by chromosomal
location, with usage like `sorted(variants, key=variant_range_tuple)`.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  A three-tuple with the same reference_name, start, and end as variant.
```

<a name="variant_type"></a>
### `variant_type(variant)`
```
Gets the VariantType of variant.

Args:
  variant: nucleus.genomics.v1.Variant.

Returns:
  VariantType indicating the type of this variant.
```

<a name="variants_are_sorted"></a>
### `variants_are_sorted(variants)`
```
Returns True if variants are sorted w.r.t. variant_range.

Args:
  variants: list[nucleus.genomics.v1.Variant]. A list of Variant
    protos that may or may not be sorted.

Returns:
  True if variants are sorted, False otherwise.
```

<a name="variants_overlap"></a>
### `variants_overlap(variant1, variant2)`
```
Returns True if the range of variant1 and variant2 overlap.

This is equivalent to:

  ranges_overlap(variant_range(variant1), variant_range(variant2))

Args:
  variant1: nucleus.genomics.v1.Variant we want to compare for overlap.
  variant2: nucleus.genomics.v1.Variant we want to compare for overlap.

Returns:
  True if the variants overlap, False otherwise.
```

