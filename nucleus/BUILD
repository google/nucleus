# Nucleus is a python library for reading and writing common genomics
# file formats.
load("//:tools/nucleus.bzl", "nucleus_py_library", "nucleus_py_extension", "nucleus_py_binary")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

nucleus_py_library(
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
