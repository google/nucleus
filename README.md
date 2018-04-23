# Nucleus

Nucleus is a library of Python and C++ code designed to make it easy to
read, write and analyze data in common genomics file formats like SAM and VCF.
In addition, Nucleus enables painless integration with the TensorFlow machine
learning framework, as anywhere a genomics file is consumed or produced, a
TensorFlow tfrecords file may be substituted.

## Poll

Which of these would most increase your usage of Nucleus?

[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20CRAM)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20CRAM/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Improved%20Speed)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Improved%20Speed/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20BigWig%E2%88%95BigBed)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20BigWig%E2%88%95BigBed/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Better%20TensorFlow%20integration)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Better%20TensorFlow%20integration/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20writing%20BAM%E2%88%95SAM%20files)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20writing%20BAM%E2%88%95SAM%20files/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Spark%20integration)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Spark%20integration/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20GTF%E2%88%95GFF)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20GTF%E2%88%95GFF/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Beam%20integration)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Beam%20integration/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20another%20unlisted%20file%20type)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Support%20for%20another%20unlisted%20file%20type/vote)
[![](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Improved%20documentation)](https://api.gh-polls.com/poll/01CBT48Z3E88TFWWH8AMFKHXD1/Improved%20documentation/vote)


## Installation

For Ubuntu 14, Ubuntu 16 and Debian 9 systems, installation is easy:
just run

```shell
source install.sh
```

For all other systems, you will need to first install CLIF by following
the instructions at [https://github.com/google/clif#installation](https://github.com/google/clif#installation) before running install.sh.

Note that install.sh extensively depends on apt-get, so it is unlikely
to run without extensive modifications on non-Debian-based systems.

Finally, Nucleus depends on TensorFlow and by default, install.sh will
install a CPU-only version of TensorFlow using a nightly wheel.  If that isn't
what you want, there are several other installation options that can be enabled
with a simple edit to install.sh.

Running ``install.sh`` will build all of Nucleus's programs and libraries.
You can find the generated  binaries under ``bazel-bin/nucleus``.  If in
addition to installing Nucleus you would like to run its tests, simply execute

```shell
bazel test -c opt $COPT_FLAGS nucleus/...
```

## Documentation

* [Overview](https://github.com/google/nucleus/blob/master/docs/overview.md).
* [Summary of example programs](https://github.com/google/nucleus/blob/master/docs/examples.md).

## Contributing

Interested in contributing? See [CONTRIBUTING](CONTRIBUTING.md).

## Support

The [Genomics team in Google Brain](https://research.google.com/teams/brain/genomics/)
actively supports Nucleus and are always interested in improving its quality.
If you run into an issue, please report the problem on our [Issue
tracker](https://github.com/google/nucleus/issues). Be sure to add enough
detail to your report that we can reproduce the problem and fix it. We encourage
including links to snippets of BAM/VCF/etc files that provoke the bug, if
possible. Depending on the severity of the issue we may patch Nucleus
immediately with the fix or roll it into the next release.

## Version

This is Nucleus 0.1.0.  Nucleus follows [semantic
versioning](https://semver.org/).

## License

Nucleus is licensed under the terms of the [Apache 2 license](LICENSE).

## History

Nucleus grew out of the [DeepVariant](https://github.com/google/deepvariant)
project.

## Disclaimer

This is not an official Google product.
