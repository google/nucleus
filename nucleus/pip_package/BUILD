# Description:
#   Tools for building the Nucleus pip package.

load("@org_tensorflow//tensorflow:tensorflow.bzl", "transitive_hdrs")

licenses(["notice"])  # Apache License 2.0

transitive_hdrs(
    name = "included_headers",
    deps = [
        "//nucleus/io:io_cpp",
        # TODO(thomaswc): Find a way to include the following two testonly
        # dependencies:
        # "//nucleus/testing:testing_cpp",
        # "//nucleus/testing:testing_py",
        "//nucleus/util:util_cpp",
        "//nucleus/vendor:vendor_cpp",
    ],
)

filegroup(
    name = "licenses",
    data = [
        # TODO(thomaswc): Find a way to include all of LICENSE files used
        # by Nucleus dependencies, including:
        # "//nucleus/opensource_only/third_party/abseil_cpp.LICENSE",
        # "//nucleus/LICENSE",
        # "//third_party/htslib/LICENSE",
        # "@clif//:LICENSE",
        # "@com_google_absl_py//:LICENSE",
        # "@com_google_googletest//:LICENSE",
        "@org_tensorflow//:LICENSE",
        "@protobuf_archive//:LICENSE",
    ],
)

PIP_DEPS = [
    ":licenses",
    ":included_headers",
    "setup.py",
    "//nucleus/io:io_py",
    "//nucleus/util:util_py",
]

sh_binary(
    name = "build_pip_package",
    srcs = ["build_pip_package.sh"],
    data = PIP_DEPS,
)
