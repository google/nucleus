# Nucleus

Nucleus is a library of Python and C++ code designed to make it easy to read,
write and analyze data in common genomics file formats like SAM and VCF. In
addition, Nucleus enables painless integration with the TensorFlow machine
learning framework, as anywhere a genomics file is consumed or produced, a
TensorFlow tfrecords file may be used instead.

## Tutorial

Please check out our tutorial on
[using Nucleus and TensorFlow for DNA sequencing error correction](https://colab.research.google.com/github/google/nucleus/blob/master/nucleus/examples/dna_sequencing_error_correction.ipynb).
It's a Python notebook that really demonstrates the power of Nucleus at
integrating information from multiple file types (BAM, VCF and Fasta) and
turning it into a form usable by TensorFlow.

## Poll

Which of these would most increase your usage of Nucleus? (Click on an option to
vote on it.)

[![](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Better%20TensorFlow%20integration)](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Better%20TensorFlow%20integration/vote)
[![](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Spark%20integration)](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Spark%20integration/vote)
[![](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Beam%20integration)](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Beam%20integration/vote)
[![](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Improved%20documentation)](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Improved%20documentation/vote)
[![](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Support%20for%20more%20file%20formats)](https://api.gh-polls.com/poll/01CQSHKQZMV3F2JZ72YYQ28Q4F/Support%20for%20more%20file%20formats/vote)

## Installation

Nucleus currently only works on modern Linux systems using Python 3. It must be
installed using a version of `pip` less than 21. To determine the version of pip
installed on your system, run

```
pip --version
```

To install Nucleus, run

```shell
pip install --user google-nucleus
```

Note that each version of Nucleus works with a specific TensorFlow version. Check the [releases](https://github.com/google/nucleus/releases) page for specifics.

You can ignore any "Failed building wheel for google-nucleus" error messages -- these are expected
and won't prevent Nucleus from installing successfully.

If you are using Python 2, instead run

```shell
pip install --user google-nucleus==0.3.2
```

## Documentation

*   [Overview](https://github.com/google/nucleus/blob/master/docs/overview.md).
*   [Summary of example programs](https://github.com/google/nucleus/blob/master/docs/examples.md).
*   [Python API Reference](https://github.com/google/nucleus/blob/master/docs/source/doc_index.md).

## Building from source

For Ubuntu 20, building from source is easy. Simply type

```shell
source install.sh
```

This will call `build_clif.sh`, which will build CLIF from scratch as well.

For all other systems, you will need to first install CLIF by following the
instructions at
[https://github.com/google/clif#installation](https://github.com/google/clif#installation)
before running install.sh. You'll need to run this command with Python 3.8.
If you don't want to build CLIF binaries on your own, you can consider
using pre-built CLIF binaries (see
[an example here](https://github.com/google/nucleus/blob/v0.5.6/install.sh#L143-L152)). Note that we don't plan to update these pre-built CLIF binaries, so we
recommend building CLIF binaries from scratch.

Note that install.sh extensively depends on apt-get, so it is unlikely to run
without extensive modifications on non-Debian-based systems.

Nucleus depends on TensorFlow. By default, install.sh will install a CPU-only
version of a stable TensorFlow release (currently 2.6). If that isn't what you
want, there are several other options that can be enabled with a simple edit to
`install.sh`.

Running `install.sh` will build all of Nucleus's programs and libraries. You can
find the generated binaries under `bazel-bin/nucleus`. If in addition to
building Nucleus you would like to run its tests, execute

```shell
bazel test -c opt $BAZEL_FLAGS nucleus/...
```

## Version

This is Nucleus 0.6.0. Nucleus follows
[semantic versioning](https://semver.org/).

New in 0.6.0:

*   Upgrade to support TensorFlow 2.6.0 specifically.
*   Upgrade to Python 3.8.

New in 0.5.9:

*   Upgrade to support TensorFlow 2.5.0 specifically.

New in 0.5.8:

*   Update `util/vis.py` to use updated channel names.
*   Support `MED_DP` (median DP) field for a `VariantCall`.

New in 0.5.7:

*   Add automatic pileup curation functionality in `util/vis.py`.
*   Upgrade protobuf settings to support TensorFlow 2.4.0 specifically.

New in 0.5.6:

*   Upgrade to protobuf 3.9.2 to support TensorFlow 2.3.0 specifically.

New in 0.5.5:

*   Upgrade protobuf settings to support TensorFlow 2.2.0 specifically.

New in 0.5.4:

*   Upgrade to protobuf 3.8.0 to support TensorFlow 2.1.0. * Add explicit
    .close() method to TFRecordWriter.

New in 0.5.3:

*   Fixes memory leaks in message_module.cc.
*   Updates setup.py to install .egg-info directory for pip 20.2+ compatibility.
*   Pins TensorFlow to 2.0.0 for protobuf version compatibility.
*   Pins setuptools to 49.6.0 to avoid breaking changes of setuptools 50.

New in 0.5.2:

*   Upgrades htslib dependency from 1.9 to 1.10.2.
*   More informative error message for failed SAM header parsing.
*   `util/vis.py` now supports saving images to Google Cloud Storage.

New in 0.5.1:

*   Added new utilities for working with DeepVariant pileup images and variant
    protos.

New in 0.5.0:

*   Fixed bug preventing Nucleus to work with TensorFlow 2.0.
*   Added util.vis routines for visualizing DeepVariant pileup examples.
*   FASTA reader now supports keep\_true\_case option for keeping the original
    casing.
*   VCF writer now supports writing headerless VCF files.
*   SAM reader now supports optional fields of type 'B'.
*   variant\_utils now supports gVCF files.
*   Numerous minor bug fixes.

New in 0.4.1:

*   Pip package is slightly more robust.

New in 0.4.0:

*   The Nucleus pip package now works with Python 3.

New in 0.3.0:

*   Reading of VCF, SAM, and most other genomics files is now twice as fast.
*   Read range and end calculations are now done in C++ for speed.
*   VcfReader can now read "headerless" VCF files.
*   variant\_utils.major\_allele\_frequency now 5x faster.
*   Memory leaks fixed in TFRecordReader/Writer and gfile\_cc.

New in 0.2.3:

*   Nucleus no longer depends on any specific version of TensorFlow's python
    code. This should make it easier to use Nucleus with for example TensorFlow
    2.0.
*   Added BCF support to VcfWriter.
*   Fixed memory leaks in VcfWriter::Write.
*   Added print\_tfrecord example program.

New in 0.2.2:

*   Faster SAM file querying and read overlap calculations.
*   Writing protocol buffers to files uses less memory.
*   Smaller pip package.
*   nucleus/util:io\_utils refactored into nucleus/io:tfrecord and
    nucleus/io:sharded\_file\_utils.
*   Alleles coming from VCF files are now always normalized as uppercase.

New in 0.2.1:

*   Upgrades htslib dependency from 1.6 to 1.9.
*   Minor VCF parsing fixes.
*   Added new example program, apply\_genotyping\_prior.
*   Slightly more robust pip package.

New in 0.2.0:

*   Support for reading and writing BedGraph files.
*   Support for reading and writing GFF files.
*   Support for reading and writing CRAM files.
*   Support for writing SAM/BAM files.
*   Support for reading unindexed FASTA files.
*   Iteration support for indexed FASTA files.
*   Ability to read VCF files from memory.
*   Python API documentation.
*   Python 3 compatibility.
*   Added universal file converter example program.

## License

Nucleus is licensed under the terms of the [Apache 2 license](LICENSE).

## Support

The
[Genomics team in Google Brain](https://research.google.com/teams/brain/genomics/)
actively supports Nucleus and are always interested in improving its quality. If
you run into an issue, please report the problem on our
[Issue tracker](https://github.com/google/nucleus/issues). Be sure to add enough
detail to your report that we can reproduce the problem and fix it. We encourage
including links to snippets of BAM/VCF/etc files that provoke the bug, if
possible. Depending on the severity of the issue we may patch Nucleus
immediately with the fix or roll it into the next release.

## Contributing

Interested in contributing? See [CONTRIBUTING](CONTRIBUTING.md).

## History

Nucleus grew out of the [DeepVariant](https://github.com/google/deepvariant)
project.

## Disclaimer

This is not an official Google product.
