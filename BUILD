load("//nucleus/tools:zip_dir.bzl", "zip_dir")
# KEEP THIS LINE to import nucleus.bzl externally

package(
    default_visibility = [
        "//visibility:public",
    ],
)

# TODO(thomaswc): Add smoke tests for Nucleus.

filegroup(
    name = "libraries",
    srcs = [
        "//nucleus",
        "//nucleus:nucleus_py",
    ],
)

exports_files(["LICENSE"])

filegroup(
    name = "licenses",
    srcs = [
        ":LICENSE",
        "//third_party:abseil_cpp.LICENSE",  # TODO(b/69856150)
        "@com_google_protobuf//:LICENSE",
        "@com_googlesource_code_re2//:LICENSE",
        "@htslib//:LICENSE",
        "@org_tensorflow//:LICENSE",
    ],
)

zip_dir(
    name = "licenses_zip",
    srcs = [":licenses"],
    zipname = "licenses.zip",
)

cc_library(
    name = "all_extensions",
    srcs = [],
    deps = [
        "//nucleus/io/python:bed_reader_cclib",
        "//nucleus/io/python:bed_writer_cclib",
        "//nucleus/io/python:bedgraph_reader_cclib",
        "//nucleus/io/python:bedgraph_writer_cclib",
        "//nucleus/io/python:fastq_reader_cclib",
        "//nucleus/io/python:fastq_writer_cclib",
        "//nucleus/io/python:gff_reader_cclib",
        "//nucleus/io/python:gff_writer_cclib",
        "//nucleus/io/python:gfile_cclib",
        "//nucleus/io/python:hts_verbose_cclib",
        "//nucleus/io/python:reference_cclib",
        "//nucleus/io/python:sam_reader_cclib",
        "//nucleus/io/python:sam_writer_cclib",
        "//nucleus/io/python:tabix_indexer_cclib",
        "//nucleus/io/python:tfrecord_reader_cclib",
        "//nucleus/io/python:tfrecord_writer_cclib",
        "//nucleus/io/python:vcf_reader_cclib",
        "//nucleus/io/python:vcf_writer_cclib",
        "//nucleus/util/python:math_cclib",
        "//nucleus/util/python:utils_cclib",
        "//nucleus/vendor/python:statusor_examples_cclib",
    ],
)
