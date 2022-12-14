package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "zlib",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    copts = select({
        "@//:windows_zig_cc": [
            "-Wno-deprecated-non-prototype",
        ],
        "@//:linux": [
            "-Wno-implicit-function-declaration",
            "-Wno-deprecated-non-prototype",
        ],
        "//conditions:default": [],
    }),
    includes = ["."],
)
