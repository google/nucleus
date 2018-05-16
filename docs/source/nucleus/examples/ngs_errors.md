# nucleus.examples.ngs_errors -- Creates tf.Example protos for learning the error process of a sequencer.
**Source code:** [nucleus/examples/ngs_errors.py](https://github.com/google/nucleus/tree/master/nucleus/examples/ngs_errors.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
This program reads in aligned NGS reads from `--bam` and emits a TFRecord of
tf.Examples containing the observed bases and quality scores for each read as
well as the true genome sequence for that read, which are determined from the
reference genome (`--ref`) and the known variants for the sample sequenced in
the BAM (`--vcf`). For example, suppose we have a read in --bam:

    sequence: ACGT
    aligned at: chr20:10-15
    cigar: 4M

And that the reference genome at chr20:10-15 is ACCT. This program first checks
that `--vcf` file doesn't have any genetic variants in the region chr20:10-15.
If there are no variants there, the program emits a tf.Example containing:

    read_sequence - the bases of the read ('ACGT')
    true_sequence - the reference genome bases ('ACCT')
    as well as read_name, cigar, and read_qualities

When run over a BAM file, we generate one tf.Example for each read that
satisfies our read requirements (e.g., aligned, not a duplicate), that spans a
region of the genome without genetic variants in `--vcf`, and spans reference
bases containing only A, C, G, or T bases.

The emitted examples are suitable (but not ideal, see below) for training a
TensorFlow model to predict the true sequence for a read. This learned model can
be considered a model for the error process of the sequencer. It can be used to
to "error correct" a read, or to compute a distribution of possible true
sequences for a given read.

NOTE: we have not trained a TensorFlow model based on this program. It is an
interesting technical / scientific question what kind of model architecture
would perform well here, and how accurately it can learn to identify errors in
the input reads. There are many extensions to this example program that would be
important to add when attempting to training a production-grade error model,
though (see next for one example).

NOTE: This program doesn't filter out reads that aren't in the confidently
determined region of the genome. A more sophisticated extension would also
accept a confident regions BED file and use ranges.RangeSet to only include
reads fully within the confident regions. This is left as an exercise for the
user.

## Prerequisites

a) Grab a slice of the BAM file via samtools. We don't need to index it because
   we are only iterating over the records, and don't need the `query()`
   functionality.

    samtools view -h \
      ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/RMNISTHS_30xdownsample.bam \
      20:10,000,000-10,100,000 \
      -o /tmp/NA12878_sliced.bam

b) Get the Genome in a Bottle truth set for NA12878 and index it with tabix.

    wget \
      ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/release/NA12878_HG001/latest/GRCh37/HG001_GRCh37_GIAB_highconf_CG-IllFB-IllGATKHC-Ion-10X-SOLID_CHROM1-X_v.3.3.2_highconf_PGandRTGphasetransfer.vcf.gz \
      -O /tmp/NA12878_calls.vcf.gz

    tabix /tmp/NA12878_calls.vcf.gz

c) Get the reference genome FASTA file from the DeepVariant testdata GCP bucket
   and index it with samtools.

    gsutil cp gs://deepvariant/case-study-testdata/hs37d5.fa.gz /tmp/hs37d5.fa.gz
    samtools faidx /tmp/hs37d5.fa.gz

## Build and run

    bazel build -c opt $COPT_FLAGS nucleus/examples:ngs_errors

    bazel-bin/nucleus/examples/ngs_errors \
      --alsologtostderr \
      --ref /tmp/hs37d5.fa.gz \
      --vcf /tmp/NA12878_calls.vcf.gz \
      --bam /tmp/NA12878_sliced.bam \
      --examples_out /tmp/examples.tfrecord

## Functions overview
Name | Description
-----|------------
[`is_usable_training_example`](#is_usable_training_example)`(read, variants, ref_bases)` | Returns True if we can use read to make a training example.
[`main`](#main)`(argv)` | 
[`make_example`](#make_example)`(read, ref_bases)` | Create a tf.Example for read and ref_bases.
[`make_ngs_error_examples`](#make_ngs_error_examples)`(ref_path, vcf_path, bam_path, examples_out_path, max_reads=None)` | Driver program for ngs_errors.

## Functions
<a name="is_usable_training_example"></a>
### `is_usable_training_example(read, variants, ref_bases)`
```
Returns True if we can use read to make a training example.

Args:
  read: nucleus.genomics.v1.Read proto.
  variants: list[nucleus.genomics.v1.Variant] protos. A list of variants
    overlapping read.
  ref_bases: str. The reference bases for read.

Returns:
  True if read can be used to construct a high-quality training example, False
  otherwise.
```

<a name="main"></a>
### `main(argv)`


<a name="make_example"></a>
### `make_example(read, ref_bases)`
```
Create a tf.Example for read and ref_bases.

Args:
  read: nucleus.genomics.v1.Read proto with cigar, fragment_name, and
    aligned_sequence.
  ref_bases: str. The reference bases for read.

Returns:
  A tf.Example protobuf with the following features:
    read_name - for debugging convenience
    cigar - the cigar string of the read
    read_sequence - the bases observed by the instrument
    read_qualities - the quality scores emitted by the instrument, as
                     phred-scaled integer values.
    true_sequence - the "true" bases that should have been observed for this
                    read, as extracted from the reference genome.
```

<a name="make_ngs_error_examples"></a>
### `make_ngs_error_examples(ref_path, vcf_path, bam_path, examples_out_path, max_reads=None)`
```
Driver program for ngs_errors.

See module description for details.

Args:
  ref_path: str. A path to an indexed fasta file.
  vcf_path: str. A path to an indexed VCF file.
  bam_path: str. A path to an SAM/BAM file.
  examples_out_path: str. A path where we will write out examples.
  max_reads: int or None. If not None, we will emit at most max_reads examples
    to examples_out_path.
```

