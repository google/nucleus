# Description:
#   Tools for building the Nucleus pip package.

load("@org_tensorflow//tensorflow:tensorflow.bzl", "transitive_hdrs")
load("//:tools/nucleus.bzl", "nucleus_py_library", "nucleus_py_extension", "nucleus_py_binary")

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
        "//third_party:abseil_cpp.LICENSE",
        "//third_party:bazel.LICENSE",
        "//third_party:clif.LICENSE",
        "//:LICENSE",
        "@htslib//:LICENSE",
        # The following dependencies contain LICENSE files, but
        # don't define proper BUILD files and/or don't export the LICENSE files.
        # TODO(thomaswc): Copy these LICENSES to third_party, or
        # write pull requests for exporting them.
        # "@com_google_absl_py//:LICENSE",
        # "@com_google_googletest//:LICENSE",
        # "@com_googlesource_code_cctz//:LICENSE.txt",
        # "@io_bazel_rules_closure//:LICENSE",
        "@org_tensorflow//:LICENSE",
        "@protobuf_archive//:LICENSE",
    ],
)

# This is a fake target, designed to populate build_pip_package.runfiles
# with the right dependency files (including the __init__.py's).  It is testonly
# so that it may depend on testonly targets in nucleus/testing.
nucleus_py_binary(
    name = "setup",
    testonly = 1,
    srcs = ["setup.py"],
    legacy_create_init = 1,
    deps = [
        "//nucleus/io:io_py",
        "//nucleus/testing:testing_py",
        "//nucleus/util:util_py",
    ],
)

PIP_DEPS = [
    ":setup",
    ":licenses",
    ":included_headers",
    "setup.py",
]

sh_binary(
    name = "build_pip_package",
    testonly = 1,
    srcs = ["build_pip_package.sh"],
    data = PIP_DEPS,
)
