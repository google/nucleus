# Nucleus contains Python and C++ code for reading, writing, and analyzing
# common genomics file formats.

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

py_library(
    name = "__init__py",
    srcs = ["__init__.py"],
    srcs_version = "PY3",
    deps = [],
)
