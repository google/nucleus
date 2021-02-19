load("//third_party:clif.bzl", "py_clif_cc")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

licenses(["notice"])  # Apache License 2.0

py_clif_cc(
    name = "math",
    srcs = ["math.clif"],
    py_deps = [],
    pyclif_deps = [],
    deps = ["//nucleus/util:cpp_math"],
)

py_test(
    name = "math_wrap_test",
    size = "small",
    srcs = ["math_wrap_test.py"],
    data = [],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":math",
        "@absl_py//absl/testing:absltest",
    ],
)

py_clif_cc(
    name = "utils",
    srcs = ["utils.clif"],
    py_deps = [],
    pyclif_deps = [
        "//nucleus/protos:range_pyclif",
        "//nucleus/protos:reads_pyclif",
    ],
    deps = [
        "//nucleus/util:cpp_utils",
        "//nucleus/util:proto_clif_converter",
    ],
)
