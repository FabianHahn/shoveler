package(default_visibility = ["//visibility:public"])

config_setting(
    name = "linux",
    constraint_values = ["@bazel_tools//platforms:linux"],
)

config_setting(
    name = "windows",
    constraint_values = [
        "@bazel_tools//platforms:windows",
        "@zig_sdk//platform:zig_cc_disabled",
    ],
)

config_setting(
    name = "windows_zig",
    constraint_values = [
        "@bazel_tools//platforms:windows",
        "@zig_sdk//platform:zig_cc_enabled",
    ],
)
