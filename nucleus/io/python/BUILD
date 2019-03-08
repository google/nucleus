load("//:third_party/clif.bzl", "py_clif_cc")
# load("//devtools/python/blaze:python3.bzl", "py2and3_test")
# KEEP THIS LINE to import nucleus.bzl externally

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
        "@absl_py//absl/testing:absltest",
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
        "//nucleus/util:proto_clif_converter",
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
        "//nucleus/io:gfile",
        "//nucleus/io:tfrecord",
        "//nucleus/io:vcf",
        "//nucleus/protos:reference_py_pb2",
        "//nucleus/protos:variants_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
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
        "//nucleus/protos:reads_py_pb2",
        "//nucleus/protos:reference_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:ranges",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "sam_writer",
    srcs = ["sam_writer.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:reads_pyclif",
    ],
    deps = [
        "//nucleus/io:sam_writer",
        "//nucleus/util:proto_clif_converter",
        "//nucleus/vendor:statusor_clif_converters",
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

py_test(
    name = "reference_wrap_test",
    size = "small",
    srcs = ["reference_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":reference",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:ranges",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
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
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
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
        "//nucleus/util:proto_clif_converter",
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
        "//nucleus/io:gfile",
        "//nucleus/io:tfrecord",
        "//nucleus/protos:bed_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "bedgraph_reader",
    srcs = ["bedgraph_reader.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:bedgraph_pyclif",
    ],
    deps = [
        "//nucleus/io:bedgraph_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_clif_cc(
    name = "bedgraph_writer",
    srcs = ["bedgraph_writer.clif"],
    pyclif_deps = [
        "//nucleus/protos:bedgraph_pyclif",
    ],
    deps = [
        "//nucleus/io:bedgraph_writer",
        "//nucleus/util:proto_clif_converter",
        "//nucleus/vendor:statusor_clif_converters",
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
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
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
        "//nucleus/util:proto_clif_converter",
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
        "//nucleus/io:gfile",
        "//nucleus/io:tfrecord",
        "//nucleus/protos:fastq_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
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
        "@absl_py//absl/testing:absltest",
    ],
)

py_clif_cc(
    name = "gff_reader",
    srcs = ["gff_reader.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:gff_pyclif",
    ],
    deps = [
        "//nucleus/io:gff_reader",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "gff_reader_wrap_test",
    size = "small",
    srcs = ["gff_reader_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":gff_reader",
        "//nucleus/protos:gff_py_pb2",
        "//nucleus/testing:py_test_utils",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "gff_writer",
    srcs = ["gff_writer.clif"],
    py_deps = [
        "//nucleus/io:clif_postproc",
    ],
    pyclif_deps = [
        "//nucleus/protos:gff_pyclif",
    ],
    deps = [
        "//nucleus/io:gff_writer",
        "//nucleus/util:proto_clif_converter",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_test(
    name = "gff_writer_wrap_test",
    size = "small",
    srcs = ["gff_writer_wrap_test.py"],
    data = ["//nucleus/testdata"],
    srcs_version = "PY2AND3",
    deps = [
        ":gff_writer",
        "//nucleus/io:tfrecord",
        "//nucleus/protos:gff_py_pb2",
        "//nucleus/testing:py_test_utils",
        "//nucleus/util:ranges",
        "@absl_py//absl/testing:absltest",
        "@absl_py//absl/testing:parameterized",
    ],
)

py_clif_cc(
    name = "tabix_indexer",
    srcs = ["tabix_indexer.clif"],
    deps = [
        "//nucleus/io:tabix_indexer",
        "//nucleus/vendor:statusor_clif_converters",
    ],
)

py_clif_cc(
    name = "tfrecord_reader",
    srcs = ["tfrecord_reader.clif"],
    deps = [
        "//nucleus/io:tfrecord_reader",
    ],
)

py_clif_cc(
    name = "tfrecord_writer",
    srcs = ["tfrecord_writer.clif"],
    deps = [
        "//nucleus/io:tfrecord_writer",
    ],
)
