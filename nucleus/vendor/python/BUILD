load("//third_party:clif.bzl", "py_clif_cc")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

licenses(["notice"])  # Apache License 2.0

py_clif_cc(
    name = "statusor_examples",
    srcs = ["statusor_examples.clif"],
    deps = [
        "//nucleus/vendor:statusor_clif_converters",
        "//nucleus/vendor:statusor_examples",
    ],
)

py_test(
    name = "statusor_examples_test",
    size = "small",
    srcs = ["statusor_examples_test.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":statusor_examples",
        "@absl_py//absl/testing:absltest",
    ],
)
