load("//:third_party/clif.bzl", "py_clif_cc")
# load("//devtools/python/blaze:python3.bzl", "py2and3_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

py_clif_cc(
    name = "vcf_reader",
    srcs = ["vcf_reader.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:range_pyclif",
        "//nucleus/protos:reference_pyclif",
        "//nucleus/protos:variants_pyclif",
    ],
    deps = [
        "//nucleus/io:vcf_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "vcf_reader_wrap_test",
    size = "small",
    srcs = ["vcf_reader_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":vcf_reader",
        "//nucleus/protos:reference_py_pb2",
        "//nucleus/protos:variants_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:ranges",
        "@com_google_absl_py//absl/testing:absltest",
    ],
)

py_clif_cc(
    name = "vcf_writer",
    srcs = ["vcf_writer.clif"],
    pyclif_deps = [
        "//nucleus/protos:variants_pyclif",
    ],
    deps = [
        "//nucleus/io:vcf_writer",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "vcf_writer_wrap_test",
    size = "small",
    srcs = ["vcf_writer_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":vcf_writer",
        "//nucleus/io:vcf",
        "//nucleus/protos:reference_py_pb2",
        "//nucleus/protos:variants_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:io_utils",
        "@com_google_absl_py//absl/testing:absltest",
        "@com_google_absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "sam_reader",
    srcs = ["sam_reader.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:range_pyclif",
        "//nucleus/protos:reads_pyclif",
        "//nucleus/protos:reference_pyclif",
    ],
    deps = [
        "//nucleus/io:sam_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "sam_reader_wrap_test",
    size = "small",
    srcs = ["sam_reader_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":sam_reader",
        "//nucleus/io:clif_postproc",
        "//nucleus/protos:index_py_pb2",
        "//nucleus/protos:reads_py_pb2",
        "//nucleus/protos:reference_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:ranges",
        "@com_google_absl_py//absl/testing:absltest",
    ],
)

py_clif_cc(
    name = "reference",
    srcs = ["reference.clif"],
    pyclif_deps = [
        "//nucleus/protos:range_pyclif",
        "//nucleus/protos:reference_pyclif",
    ],
    deps = [
        "//nucleus/io:reference",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_clif_cc(
    name = "reference_fai",
    srcs = ["reference_fai.clif"],
    clif_deps = [
        ":reference",
    ],
    deps = [
        "//nucleus/io:reference_fai",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "reference_wrap_test",
    size = "small",
    srcs = ["reference_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":reference_fai",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:ranges",
        "@com_google_absl_py//absl/testing:absltest",
        "@com_google_absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "fasta_reader",
    srcs = ["fasta_reader.clif"],
    pyclif_deps = [
        "//nucleus/protos:reference_pyclif",
    ],
    deps = [
        "//nucleus/io:fasta_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_clif_cc(
    name = "bed_reader",
    srcs = ["bed_reader.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:bed_pyclif",
    ],
    deps = [
        "//nucleus/io:bed_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "bed_reader_wrap_test",
    size = "small",
    srcs = ["bed_reader_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":bed_reader",
        "//nucleus/protos:bed_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@com_google_absl_py//absl/testing:absltest",
        "@com_google_absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "bed_writer",
    srcs = ["bed_writer.clif"],
    pyclif_deps = [
        "//nucleus/protos:bed_pyclif",
    ],
    deps = [
        "//nucleus/io:bed_writer",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "bed_writer_wrap_test",
    size = "small",
    srcs = ["bed_writer_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":bed_writer",
        "//nucleus/protos:bed_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:io_utils",
        "@com_google_absl_py//absl/testing:absltest",
        "@com_google_absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "fastq_reader",
    srcs = ["fastq_reader.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:fastq_pyclif",
    ],
    deps = [
        "//nucleus/io:fastq_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "fastq_reader_wrap_test",
    size = "small",
    srcs = ["fastq_reader_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":fastq_reader",
        "//nucleus/protos:fastq_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@com_google_absl_py//absl/testing:absltest",
        "@com_google_absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "fastq_writer",
    srcs = ["fastq_writer.clif"],
    pyclif_deps = [
        "//nucleus/protos:fastq_pyclif",
    ],
    deps = [
        "//nucleus/io:fastq_writer",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "fastq_writer_wrap_test",
    size = "small",
    srcs = ["fastq_writer_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":fastq_writer",
        "//nucleus/io:fastq",
        "//nucleus/protos:fastq_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:io_utils",
        "@com_google_absl_py//absl/testing:absltest",
        "@com_google_absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "hts_verbose",
    srcs = ["hts_verbose.clif"],
    deps = [
        "//nucleus/io:hts_verbose",
    ],
)

py_test(
    name = "hts_verbose_test",
    size = "small",
    srcs = ["hts_verbose_test.py"],
    srcs_version = "PY2AND3",
    deps = [
        ":hts_verbose",
        "@com_google_absl_py//absl/testing:absltest",
    ],
)
