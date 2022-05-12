package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "glfw",
    srcs = [
        "src/context.c",
        "src/egl_context.c",
        "src/egl_context.h",
        "src/init.c",
        "src/input.c",
        "src/internal.h",
        "src/mappings.h",
        "src/monitor.c",
        "src/osmesa_context.c",
        "src/osmesa_context.h",
        "src/vulkan.c",
        "src/window.c",
    ] + select({
        "@//:windows": [
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_joystick.h",
            "src/win32_monitor.c",
            "src/win32_platform.h",
            "src/win32_time.c",
            "src/win32_thread.c",
            "src/win32_window.c",
            "src/wgl_context.c",
            "src/wgl_context.h",
        ],
        "@//:linux": [
            "src/glx_context.c",
            "src/glx_context.h",
            "src/linux_joystick.c",
            "src/linux_joystick.h",
            "src/posix_time.c",
            "src/posix_time.h",
            "src/posix_thread.c",
            "src/posix_thread.h",
            "src/x11_init.c",
            "src/x11_monitor.c",
            "src/x11_platform.h",
            "src/x11_window.c",
            "src/xkb_unicode.c",
            "src/xkb_unicode.h",
        ],
    }),
    hdrs = [
        "include/GLFW/glfw3.h",
        "include/GLFW/glfw3native.h",
    ],
    copts = select({
        "@//:windows": [],
        "@//:linux": [
            "-pthread",
        ],
    }),
    defines = select({
        "@//:windows": [],
        "@//:linux": [
            "_GLFW_X11",
        ],
    }),
    linkopts = select({
        "@//:windows": [],
        "@//:linux": [
            "-ldl",
            "-lX11",
            "-pthread",
        ],
    }),
    strip_include_prefix = "include",
)
