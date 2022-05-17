package(default_visibility = ["//visibility:public"])

licenses(["notice"])

genrule(
    name = "libpng_pnglibconf",
    srcs = ["scripts/pnglibconf.h.prebuilt"],
    outs = ["pnglibconf.h"],
    cmd = "cp $< $@",
)

cc_library(
    name = "libpng",
    srcs = [
        "png.c",
        "pngerror.c",
        "pngget.c",
        "pngmem.c",
        "pngpread.c",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c",
    ],
    hdrs = [
        "png.h",
        "pngconf.h",
        "pngdebug.h",
        "pnginfo.h",
        "pngpriv.h",
        "pngstruct.h",
        ":pnglibconf.h",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = ["@zlib"],
)
