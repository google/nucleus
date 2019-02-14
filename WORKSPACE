workspace(name = "nucleus")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

# Note: absl_py and com_google_absl (the Python and C++ abseil libraries) are
# provided by TensorFlow.

# CCTZ (Time-zone framework).
# TODO(b/80245980): transitive WORKSPACE dependency resolution doesn't
# work in bazel, so we need to include this to enable use of
# //absl/{time,synchronization}
http_archive(
    name = "com_googlesource_code_cctz",
    strip_prefix = "cctz-master",
    urls = ["https://github.com/google/cctz/archive/master.zip"],
)

# This is the 1.9 release of htslib.
http_archive(
    name = "htslib",
    build_file = "//:third_party/htslib.BUILD",
    sha256 = "c4d3ae84014f8a80f5011521f391e917bc3b4f6ebd78e97f238472e95849ec14",
    strip_prefix = "htslib-1.9",
    urls = [
        "https://github.com/samtools/htslib/archive/1.9.zip"
    ],
)

# Import tensorflow.  Note path.
local_repository(
    name = "org_tensorflow",
    path = "../tensorflow",
)

# Required boilerplate for tf_workspace(), apparently.
# This is copied from https://github.com/tensorflow/tensorflow/blob/v1.11.0/WORKSPACE.
http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "a38539c5b5c358548e75b44141b4ab637bba7c4dc02b46b1f62a96d6433f56ae",
    strip_prefix = "rules_closure-dbb96841cc0a5fb2664c37822803b06dab20c7d1",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_closure/archive/dbb96841cc0a5fb2664c37822803b06dab20c7d1.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/dbb96841cc0a5fb2664c37822803b06dab20c7d1.tar.gz",  # 2018-04-13
    ],
)

# We need a protobuf version at this hash or later because we need the API
# introduced in
# https://github.com/google/protobuf/pull/4698 with bug fixes at
# 4725, 4835, and 4836.
# We also use our own BUILD file to take advantage of the nucleus_py_* targets.
# It must be kept in sync with the version of protobuf used.
http_archive(
    name = "protobuf_archive",
    build_file = "//:third_party/protobuf.BUILD",
    sha256 = "ab811441e16acd6e6d19abb9fd266b0acbd7c14be331de9da7f0bdb3683ae39f",
    strip_prefix = "protobuf-79700b56b99fa5c8c22ddef78e6c9557ff711379",
    urls = [
        # TODO(thomaswc): Restore this URL when it is up on the mirror.
        # "https://mirror.bazel.build/github.com/google/protobuf/archive/a0e82dbe569552ac848d088391b63aaa1108d1a3.tar.gz",
        "https://github.com/google/protobuf/archive/79700b56b99fa5c8c22ddef78e6c9557ff711379.tar.gz",
    ],
)

# bazel_skylib is now a required dependency of protobuf_archive.
http_archive(
    name = "bazel_skylib",
    sha256 = "bbccf674aa441c266df9894182d80de104cabd19be98be002f6d478aaa31574d",
    strip_prefix = "bazel-skylib-2169ae1c374aab4a09aa90e65efe1a3aad4e279b",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/2169ae1c374aab4a09aa90e65efe1a3aad4e279b.tar.gz"],
)

# Import all of the tensorflow dependencies.
load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")

tf_workspace(tf_repo_name = "org_tensorflow")

new_local_repository(
    name = "clif",
    build_file = "third_party/clif.BUILD",
    path = "/usr/local",
)
