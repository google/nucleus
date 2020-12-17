workspace(name = "nucleus")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

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

# This is the 1.10.2 release of htslib.
http_archive(
    name = "htslib",
    build_file = "//:third_party/htslib.BUILD",
    sha256 = "f7994e9636f8a4032dea477a8613f5f73b330c23b5538e45666ce7306240ac14",
    strip_prefix = "htslib-1.10.2",
    urls = [
        "https://github.com/samtools/htslib/archive/1.10.2.zip",
    ],
)

# Import tensorflow. Note path. (This is generated as part of ./install.sh).
local_repository(
    name = "org_tensorflow",
    path = "../tensorflow",
)

# Required boilerplate for tf_workspace(), apparently.
# This is copied from https://github.com/tensorflow/tensorflow/blob/v2.4.0/WORKSPACE.
http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
    strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",  # 2019-06-13
    ],
)

# This needs to be in sync with the version of protobuf used by TensorFlow,
# which is currently defined in @tensorflow/tensorflow/workspace.bzl.
# We supply our own BUILD file, though, so we can prevent ODR violations by
# putting all of Nucleus's C++ binary dependencies into a single library.
# That BUILD file must be kept in sync with the version of protobuf used.
http_archive(
    name = "com_google_protobuf",
    build_file = "//:third_party/protobuf.BUILD",
    sha256 = "cfcba2df10feec52a84208693937c17a4b5df7775e1635c1e3baffc487b24c9b",
    # This protobuf release is based on protobuf 3.9.2.
    strip_prefix = "protobuf-3.9.2",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/protocolbuffers/protobuf/archive/v3.9.2.zip",
        "https://github.com/protocolbuffers/protobuf/archive/v3.9.2.zip",
    ],
)

# bazel_skylib is now a required dependency of com_google_protobuf.
http_archive(
    name = "bazel_skylib",
    sha256 = "97e70364e9249702246c0e9444bccdc4b847bed1eb03c5a3ece4f83dfe6abc44",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.2/bazel-skylib-1.0.2.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.2/bazel-skylib-1.0.2.tar.gz",
    ],
)

# six is now a required dependency of com_google_protobuf.
http_archive(
    name = "six",
    build_file = "//:third_party/six.BUILD",
    sha256 = "105f8d68616f8248e24bf0e9372ef04d3cc10104f1980f54d57b2ce73a5ad56a",
    urls = [
        "http://mirror.bazel.build/pypi.python.org/packages/source/s/six/six-1.10.0.tar.gz",
        "https://pypi.python.org/packages/source/s/six/six-1.10.0.tar.gz",
    ],
)

# Import all of the tensorflow dependencies.
load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")

tf_workspace(tf_repo_name = "org_tensorflow")

new_local_repository(
    name = "clif",
    build_file = "third_party/clif.BUILD",
    path = "/usr/local",
)
