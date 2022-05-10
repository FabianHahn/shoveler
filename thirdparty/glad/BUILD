package(default_visibility = ["//visibility:public"])

cc_library(
    name = "glad",
    srcs = [
        "src/glad.c",
    ],
    hdrs = [
        "include/KHR/khrplatform.h",
        "include/glad/glad.h",
    ],
    copts = select({
        "@//:windows": [],
        "//conditions:default": [
            "-Wno-unused",
        ],
    }),
    includes = ["include"],
)
