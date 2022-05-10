package(default_visibility = ["//visibility:public"])

config_setting(
    name = "linux",
    constraint_values = ["@bazel_tools//platforms:linux"],
)

config_setting(
    name = "windows",
    constraint_values = ["@bazel_tools//platforms:windows"],
)

genrule(
    name = "fakeglib_api",
    outs = ["lib/include/fakeglib/api.h"],
    cmd = "touch $@",
)

cc_library(
    name = "fakeglib",
    srcs = [
        "lib/src/GArray.cpp",
        "lib/src/GDateTime.cpp",
        "lib/src/GHashTable.cpp",
        "lib/src/GList.cpp",
        "lib/src/GMemory.cpp",
        "lib/src/GQueue.cpp",
        "lib/src/GString.cpp",
        "lib/src/GStringUtil.cpp",
        "lib/src/GTime.cpp",
    ],
    hdrs = [
        ":lib/include/fakeglib/api.h",
        "lib/include/fakeglib/GArray.h",
        "lib/include/fakeglib/GDateTime.h",
        "lib/include/fakeglib/GHashTable.h",
        "lib/include/fakeglib/GList.h",
        "lib/include/fakeglib/GMemory.h",
        "lib/include/fakeglib/GQueue.h",
        "lib/include/fakeglib/GString.h",
        "lib/include/fakeglib/GStringUtil.h",
        "lib/include/fakeglib/GTime.h",
        "lib/include/fakeglib/GTypes.h",
        "lib/include/glib.h",
    ],
    copts = select({
        "@//:windows": [],
        "//conditions:default": [
            "-Wno-sign-compare",
            "-Wno-unused",
        ],
    }),
    defines = ["FAKEGLIB_API=''"],
    includes = ["lib/include"],
    strip_include_prefix = "lib/include",
)

cc_test(
    name = "fakeglib_tests",
    srcs = [
        "test/GArrayTest.cpp",
        "test/GDateTimeTest.cpp",
        "test/GHashTableTest.cpp",
        "test/GListTest.cpp",
        "test/GQueueTest.cpp",
        "test/GStringTest.cpp",
        "test/GStringUtilTest.cpp",
        "test/GTimeTest.cpp",
        "test/main.cpp",
    ],
    deps = [
        ":fakeglib",
        "@googletest//:gtest",
    ]
)
