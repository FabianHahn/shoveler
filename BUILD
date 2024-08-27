load("@bazel_skylib//lib:selects.bzl", "selects")

package(default_visibility = ["//visibility:public"])

constraint_setting(
    name = "windows_using_zig_cc",
    default_constraint_value = "windows_zig_cc_disabled",
)

constraint_value(
    name = "windows_zig_cc_disabled",
    constraint_setting = "windows_using_zig_cc",
)

constraint_value(
    name = "windows_zig_cc_enabled",
    constraint_setting = "windows_using_zig_cc",
)

platform(
    name = "windows_zig_cc_amd64",
    constraint_values = [
        ":windows_zig_cc_enabled",
    ],
    parents = [
        "@zig_sdk//platform:windows_amd64",
    ],
)

config_setting(
    name = "linux",
    constraint_values = ["@platforms//os:linux"],
)

config_setting(
    name = "windows_msvc",
    constraint_values = [
        "@platforms//os:windows",
        ":windows_zig_cc_disabled",
    ],
)

config_setting(
    name = "windows_zig_cc",
    constraint_values = [
        "@platforms//os:windows",
        ":windows_zig_cc_enabled",
    ],
)

selects.config_setting_group(
    name = "windows",
    match_any = [
        ":windows_msvc",
        ":windows_zig_cc",
    ],
)
