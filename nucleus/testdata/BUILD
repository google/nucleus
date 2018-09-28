package(
    default_visibility = [
        "//visibility:public",
    ],
)

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

load("//:tools/nucleus.bzl", "nucleus_py_library", "nucleus_py_extension", "nucleus_py_binary")

filegroup(
    name = "testdata",
    srcs = glob([
        "*",
        "*/*",
    ]),
)
