# Nucleus is a python library for reading and writing common genomics
# file formats.

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

py_library(
    name = "nucleus_py",
    srcs = [],
    deps = [
        "//nucleus/io:bed",
        "//nucleus/io:fasta",
        "//nucleus/io:fastq",
        "//nucleus/io:genomics_reader",
        "//nucleus/io:genomics_writer",
        "//nucleus/io:sam",
        "//nucleus/io:vcf",
    ],
)

cc_library(
    name = "nucleus",
    srcs = [],
    deps = [
        "//nucleus/io:bed_reader",
        "//nucleus/io:bed_writer",
        "//nucleus/io:fastq_reader",
        "//nucleus/io:fastq_writer",
        "//nucleus/io:hts_path",
        "//nucleus/io:hts_verbose",
        "//nucleus/io:reader_base",
        "//nucleus/io:reference",
        "//nucleus/io:reference_fai",
        "//nucleus/io:sam_reader",
        "//nucleus/io:vcf_conversion",
        "//nucleus/io:vcf_reader",
        "//nucleus/io:vcf_writer",
        "//nucleus/util:cpp_math",
        "//nucleus/util:cpp_utils",
        "//nucleus/util:port",
        "//nucleus/util:samplers",
    ],
)
