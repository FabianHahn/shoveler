package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "zlib",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    copts = select({
        "@//:windows": [],
        "//conditions:default": [
            "-Wno-implicit-function-declaration",
        ],
    }),
    includes = ["."],
)
