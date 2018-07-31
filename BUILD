load("//tools:zip_dir.bzl", "zip_dir")
# KEEP THIS LINE to import nucleus.bzl externally

package(
    default_visibility = [
        "//visibility:public",
    ],
)

# TODO(thomaswc): Add smoke tests for Nucleus.

filegroup(
    name = "libraries",
    srcs = [
        "//third_party/nucleus",
        "//third_party/nucleus:nucleus_py",
    ],
)

exports_files(["LICENSE"])

filegroup(
    name = "licenses",
    srcs = [
        ":LICENSE",
        "//third_party:abseil_cpp.LICENSE",  # TODO(b/69856150)
        "@com_google_protobuf//:LICENSE",
        "@com_googlesource_code_re2//:LICENSE",
        "@htslib//:LICENSE",
        "@org_tensorflow//:LICENSE",
    ],
)

zip_dir(
    name = "licenses_zip",
    srcs = [":licenses"],
    zipname = "licenses.zip",
)
