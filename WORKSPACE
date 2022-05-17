load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_skylib",
    sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    urls = [
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

load("@bazel_skylib//lib:versions.bzl", "versions")

versions.check(
    maximum_bazel_version = "5.1.1",
    minimum_bazel_version = "5.1.1",
)

# We make thirdparty/ its own bazel workspace because it allows to run `bazel build ...` without
# having all targets defined in third-party BUILD files in this directory buildable.
local_repository(
    name = "thirdparty",
    path = "thirdparty",
)

http_archive(
    name = "fakeglib",
    sha256 = "9e28f304c01493f13d46027330b04f3936bf3ca2c47815664cd8d9b5d113584c",
    strip_prefix = "fakeglib-823da8d10e96d62286fb538b70863366fd43b505",
    url = "https://github.com/FabianHahn/fakeglib/archive/823da8d10e96d62286fb538b70863366fd43b505.tar.gz",
)

http_archive(
    name = "freetype",
    build_file = "@thirdparty//freetype:BUILD",
    sha256 = "11b13952b78e55f399a5f185c9c46e3bec0da3989066ce76c984d446a8ef7090",
    strip_prefix = "freetype-VER-2-12-0",
    url = "https://github.com/freetype/freetype/archive/VER-2-12-0.tar.gz",
)

http_archive(
    name = "glfw",
    build_file = "@thirdparty//glfw:BUILD",
    sha256 = "fd21a5f65bcc0fc3c76e0f8865776e852de09ef6fbc3620e09ce96d2b2807e04",
    strip_prefix = "glfw-3.3.7",
    url = "https://github.com/glfw/glfw/archive/3.3.7.tar.gz",
)

http_archive(
    name = "googletest",
    sha256 = "2162f7f52bbefbb921db49730e9a02f817f2c01b0912674472875f40ccee03eb",
    strip_prefix = "googletest-bf66935e07825318ae519675d73d0f3e313b3ec6",
    url = "https://github.com/google/googletest/archive/bf66935e07825318ae519675d73d0f3e313b3ec6.tar.gz",
)

http_archive(
    name = "libpng",
    build_file = "@thirdparty//libpng:BUILD",
    sha256 = "ca74a0dace179a8422187671aee97dd3892b53e168627145271cad5b5ac81307",
    strip_prefix = "libpng-1.6.37",
    url = "https://github.com/glennrp/libpng/archive/v1.6.37.tar.gz",
)

http_archive(
    name = "x11-linux-x86_x64",
    build_file = "@thirdparty//x11-linux-x86_64:BUILD",
    sha256 = "e97fe7c361624f9e1ee9f569d78e300d994cd10c38cf3b98c5ffe763911231a1",
    strip_prefix = "sdk-linux-x86_64-ab7fa8f3a05b06e0b06f4277b484e27004bfb20f",
    url = "https://github.com/hexops/sdk-linux-x86_64/archive/ab7fa8f3a05b06e0b06f4277b484e27004bfb20f.tar.gz",
)

http_archive(
    name = "zlib",
    build_file = "@thirdparty//zlib:BUILD",
    sha256 = "d8688496ea40fb61787500e863cc63c9afcbc524468cedeb478068924eb54932",
    strip_prefix = "zlib-1.2.12",
    url = "https://github.com/madler/zlib/archive/v1.2.12.tar.gz",
)

BAZEL_ZIG_CC_VERSION = "v0.7.2"

http_archive(
    name = "bazel-zig-cc",
    sha256 = "8f198ee98fa122d299b008db281329b80147d04fcdab03a55c31007b5edc8fde",
    strip_prefix = "bazel-zig-cc-{}".format(BAZEL_ZIG_CC_VERSION),
    urls = ["https://git.sr.ht/~motiejus/bazel-zig-cc/archive/{}.tar.gz".format(BAZEL_ZIG_CC_VERSION)],
)

load("@bazel-zig-cc//toolchain:defs.bzl", zig_toolchains = "toolchains")

zig_toolchains()
