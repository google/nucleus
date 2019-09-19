# Testing utilities that are temporarily located here until they can
# be separately open source.
# load("//devtools/python/blaze:python3.bzl", "py2and3_test")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

exports_files(["LICENSE"])

test_suite(
    name = "smoke_tests",
    tests = [
        "gunit_extras_test",
    ],
)

cc_library(
    name = "testing_cpp",
    testonly = True,
    deps = [
        ":cpp_test_utils",
        ":gunit_extras",
    ],
)

py_library(
    name = "testing_py",
    testonly = True,
    deps = [
        ":py_test_utils",
    ],
)

cc_library(
    name = "gunit_extras",
    testonly = True,
    srcs = [
        "protocol-buffer-matchers.cc",
    ],
    hdrs = [
        "protocol-buffer-matchers.h",
    ],
    deps = [
        "//nucleus/platform:types",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
        "@org_tensorflow//tensorflow/core:lib",
        "@org_tensorflow//tensorflow/core:test",
        "@protobuf_archive//:protobuf",
    ],
)

cc_test(
    name = "gunit_extras_test",
    size = "small",
    srcs = ["gunit_extras_test.cc"],
    deps = [
        ":gunit_extras",
        "@com_google_googletest//:gtest_main",
        "@protobuf_archive//:protobuf",
    ],
)

cc_library(
    name = "cpp_test_utils",
    testonly = True,
    srcs = ["test_utils.cc"],
    hdrs = ["test_utils.h"],
    deps = [
        "//nucleus/io:reader_base",
        "//nucleus/platform:types",
        "//nucleus/protos:cigar_cc_pb2",
        "//nucleus/protos:reads_cc_pb2",
        "//nucleus/protos:reference_cc_pb2",
        "//nucleus/util:cpp_utils",
        "//nucleus/vendor:statusor",
        "@com_google_absl//absl/strings",
        "@org_tensorflow//tensorflow/core:lib",
        "@org_tensorflow//tensorflow/core:test",
    ],
)

py_library(
    name = "py_test_utils",
    testonly = True,
    srcs = ["test_utils.py"],
    deps = [
        "//nucleus/io:gfile",
        "//nucleus/protos:position_py_pb2",
        "//nucleus/protos:reads_py_pb2",
        "//nucleus/protos:struct_py_pb2",
        "//nucleus/protos:variants_py_pb2",
        "//nucleus/util:cigar",
        "@absl_py//absl/flags",
    ],
)

py_test(
    name = "py_test_utils_test",
    size = "small",
    srcs = ["test_utils_test.py"],
    main = "test_utils_test.py",
    python_version = "PY3",
    # py3_tags = [],
    deps = [
        ":py_test_utils",
        "//nucleus/protos:cigar_py_pb2",
        "@absl_py//absl/testing:absltest",
    ],
)

py_test(
    name = "protobuf_implementation_test",
    size = "small",
    srcs = ["protobuf_implementation_test.py"],
    python_version = "PY3",
    # py3_tags = [],
    deps = [
        "//nucleus/io:sam",
        "@absl_py//absl/testing:absltest",
        "@protobuf_archive//:protobuf_python",
    ],
)
