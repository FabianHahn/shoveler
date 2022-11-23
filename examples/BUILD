package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "canvas",
    srcs = [
        "canvas.c",
    ],
    deps = [
        "//opengl",
    ],
)

cc_binary(
    name = "canvas_layers",
    srcs = [
        "canvas_layers.c",
    ],
    deps = [
        "//opengl",
    ],
)

cc_binary(
    name = "client",
    srcs = [
        "client.c",
    ],
    deps = [
        "//client",
    ],
)

cc_binary(
    name = "client_text",
    srcs = [
        "client_text.c",
    ],
    deps = [
        "//client",
    ],
)

cc_binary(
    name = "font",
    srcs = [
        "font.c",
    ],
    deps = [
        "//base",
    ],
)

genrule(
    name = "font_runner",
    srcs = [
        "@shoveler_assets//:OpenSans.ttf",
    ],
    outs = ["font_runner.dummy_out"],
    cmd = "$(location :font) $(location @shoveler_assets//:OpenSans.ttf) && touch $@",
    exec_tools = [":font"],
    executable = True,
)

cc_binary(
    name = "lights",
    srcs = [
        "lights.c",
    ],
    deps = [
        "//opengl",
    ],
)

cc_binary(
    name = "text",
    srcs = [
        "text.c",
    ],
    deps = [
        "//opengl",
    ],
)

cc_binary(
    name = "tiles",
    srcs = [
        "tiles.c",
    ],
    deps = [
        "//opengl",
    ],
)
