# nucleus.examples.add_ad_to_vcf -- This example program adds the AD info field to a VCF file.
**Source code:** [nucleus/examples/add_ad_to_vcf.py](https://github.com/google/nucleus/tree/master/nucleus/examples/add_ad_to_vcf.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
It assumes that the AD field of the individual variant calls is already
populated.

Sample usage:
  $ add_ad_to_vcf input.vcf.gz output.vcf.gz

## Functions overview
Name | Description
-----|------------
[`get_variant_ad`](#get_variant_ad)`(variant)` | Returns the allele depth for the Variant, calculated across its calls.
[`main`](#main)`(argv)` | 

## Functions
<a name="get_variant_ad"></a>
### `get_variant_ad(variant)`
```
Returns the allele depth for the Variant, calculated across its calls.
```

<a name="main"></a>
### `main(argv)`


