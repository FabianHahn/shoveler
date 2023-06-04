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
    sha256 = "65a1e0e38910d7c47d3932a61adb4def3035bec67adccf7e2818041b4b015cc9",
    strip_prefix = "googletest-97459e54ec4f1882d219c413c929f719d9741ada",
    url = "https://github.com/google/googletest/archive/97459e54ec4f1882d219c413c929f719d9741ada.tar.gz",
)

http_archive(
    name = "libpng",
    build_file = "@thirdparty//libpng:BUILD",
    sha256 = "ca74a0dace179a8422187671aee97dd3892b53e168627145271cad5b5ac81307",
    strip_prefix = "libpng-1.6.37",
    url = "https://github.com/glennrp/libpng/archive/v1.6.37.tar.gz",
)

http_archive(
    name = "shoveler_assets",
    build_file = "@thirdparty//shoveler_assets:BUILD",
    sha256 = "4d433cc2c1521d1d5e8a1ca2ae180547faa958c853cbff5399e610aede351452",
    strip_prefix = "shoveler-assets-419e095beda1f6f43dedeb3957ddf5630913d34d",
    url = "https://github.com/FabianHahn/shoveler-assets/archive/419e095beda1f6f43dedeb3957ddf5630913d34d.tar.gz",
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

HERMETIC_CC_TOOLCHAIN_VERSION = "v2.0.0"

http_archive(
    name = "hermetic_cc_toolchain",
    sha256 = "57f03a6c29793e8add7bd64186fc8066d23b5ffd06fe9cc6b0b8c499914d3a65",
    urls = [
        "https://mirror.bazel.build/github.com/uber/hermetic_cc_toolchain/releases/download/{0}/hermetic_cc_toolchain-{0}.tar.gz".format(HERMETIC_CC_TOOLCHAIN_VERSION),
        "https://github.com/uber/hermetic_cc_toolchain/releases/download/{0}/hermetic_cc_toolchain-{0}.tar.gz".format(HERMETIC_CC_TOOLCHAIN_VERSION),
    ],
)

load("@hermetic_cc_toolchain//toolchain:defs.bzl", zig_toolchains = "toolchains")

zig_toolchains()
