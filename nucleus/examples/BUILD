# Example programs for demonstrating Nucleus's APIs.
# load("//devtools/python/blaze:python3.bzl", "py2and3_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

py_library(
    name = "examples_py",
    deps = [
        ":add_ad_to_vcf_lib",
        ":apply_genotyping_prior_lib",
        ":ascii_pileup_lib",
        ":count_variants_lib",
        ":filter_vcf_lib",
        ":validate_vcf_lib",
    ],
)

py_binary(
    name = "add_ad_to_vcf",
    srcs = ["add_ad_to_vcf.py"],
    default_python_version = "PY2",
    deps = [":add_ad_to_vcf_lib"],
)

py_library(
    name = "add_ad_to_vcf_lib",
    srcs = ["add_ad_to_vcf.py"],
    deps = [
        "//nucleus/io:vcf",
        "//nucleus/util:variant_utils",
        "//nucleus/util:variantcall_utils",
        "//nucleus/util:vcf_constants",
        "@absl_py//absl:app",
    ],
)

py_test(
    name = "add_ad_to_vcf_test",
    size = "small",
    srcs = ["add_ad_to_vcf_test.py"],
    data = [
        "//nucleus/testdata",
    ],
    srcs_version = "PY2AND3",
    deps = [
        ":add_ad_to_vcf_lib",
        "//nucleus/io:vcf",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
    ],
)

py_binary(
    name = "ascii_pileup",
    srcs = ["ascii_pileup.py"],
    default_python_version = "PY2",
    deps = [":ascii_pileup_lib"],
)

py_library(
    name = "ascii_pileup_lib",
    srcs = ["ascii_pileup.py"],
    deps = [
        "//nucleus/io:sam",
        "//nucleus/util:ranges",
        "@absl_py//absl:app",
    ],
)

py_test(
    name = "ascii_pileup_test",
    size = "small",
    srcs = ["ascii_pileup_test.py"],
    data = [
        "//nucleus/testdata",
    ],
    srcs_version = "PY2AND3",
    deps = [
        ":ascii_pileup_lib",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
    ],
)

py_binary(
    name = "count_variants",
    srcs = ["count_variants.py"],
    default_python_version = "PY2",
    deps = [":count_variants_lib"],
)

py_library(
    name = "count_variants_lib",
    srcs = ["count_variants.py"],
    deps = [
        "//nucleus/io:vcf",
        "//nucleus/util:variant_utils",
        "@absl_py//absl:app",
    ],
)

py_test(
    name = "count_variants_test",
    size = "small",
    srcs = ["count_variants_test.py"],
    data = [
        "//nucleus/testdata",
    ],
    deps = [
        ":count_variants_lib",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
    ],
)

py_binary(
    name = "filter_vcf",
    srcs = ["filter_vcf.py"],
    default_python_version = "PY2",
    deps = [":filter_vcf_lib"],
)

py_library(
    name = "filter_vcf_lib",
    srcs = ["filter_vcf.py"],
    deps = [
        "//nucleus/io:vcf",
        "@absl_py//absl:app",
    ],
)

py_test(
    name = "filter_vcf_test",
    size = "small",
    srcs = ["filter_vcf_test.py"],
    data = [
        "//nucleus/testdata",
    ],
    deps = [
        ":filter_vcf_lib",
        "//nucleus/io:vcf",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
    ],
)

py_binary(
    name = "validate_vcf",
    srcs = ["validate_vcf.py"],
    default_python_version = "PY2",
    deps = [":validate_vcf_lib"],
)

py_library(
    name = "validate_vcf_lib",
    srcs = ["validate_vcf.py"],
    deps = [
        "//nucleus/io:fasta",
        "//nucleus/io:vcf",
        "//nucleus/util:variant_utils",
        "@absl_py//absl:app",
    ],
)

py_test(
    name = "validate_vcf_test",
    size = "small",
    srcs = ["validate_vcf_test.py"],
    data = [
        "//nucleus/testdata",
    ],
    deps = [
        ":validate_vcf_lib",
        "//nucleus/protos:reference_py_pb2",
        "//nucleus/protos:variants_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
    ],
)

py_binary(
    name = "apply_genotyping_prior",
    srcs = ["apply_genotyping_prior.py"],
    default_python_version = "PY2",
    deps = [":apply_genotyping_prior_lib"],
)

py_library(
    name = "apply_genotyping_prior_lib",
    srcs = ["apply_genotyping_prior.py"],
    deps = [
        "//nucleus/io:vcf",
        "//nucleus/util:variant_utils",
        "//nucleus/util:variantcall_utils",
        "@absl_py//absl:app",
    ],
)

py_test(
    name = "apply_genotyping_prior_test",
    size = "small",
    srcs = ["apply_genotyping_prior_test.py"],
    srcs_version = "PY2AND3",
    deps = [
        ":apply_genotyping_prior_lib",
        "//nucleus/protos:variants_py_pb2",
        "@absl_py//absl/testing:absltest",
    ],
)

py_binary(
    name = "print_tfrecord",
    srcs = ["print_tfrecord.py"],
    default_python_version = "PY2",
    deps = [
        "//nucleus/io:genomics_reader",
        "//nucleus/protos:bed_py_pb2",
        "//nucleus/protos:bedgraph_py_pb2",
        "//nucleus/protos:example_py_pb2",
        "//nucleus/protos:fasta_py_pb2",
        "//nucleus/protos:fastq_py_pb2",
        "//nucleus/protos:gff_py_pb2",
        "//nucleus/protos:reads_py_pb2",
        "//nucleus/protos:variants_py_pb2",
        "@protobuf_archive//:protobuf_python",
    ],
)
