# Nucleus is a python library for reading and writing common genomics
# file formats.
# KEEP THIS LINE to import nucleus.bzl externally

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

py_library(
    name = "nucleus_py",
    srcs = [],
    deps = [
        "//nucleus/io:io_py",
        "//nucleus/util:util_py",
    ],
)

cc_library(
    name = "nucleus",
    srcs = [],
    deps = [
        "//nucleus/io:io_cpp",
        "//nucleus/util:util_cpp",
    ],
)
