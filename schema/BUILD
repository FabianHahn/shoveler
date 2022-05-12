package(default_visibility = ["//visibility:public"])

cc_library(
    name = "schema",
    srcs = [
        "src/schema/base.c",
        "src/schema/components.c",
        "src/schema/opengl.c",
        "src/tiles/seeder.c",
        "src/tiles/tileset.c",
    ],
    hdrs = [
        "include/shoveler/schema/base.h",
        "include/shoveler/schema/components.h",
        "include/shoveler/schema/opengl.h",
        "include/shoveler/tiles/seeder.h",
        "include/shoveler/tiles/tileset.h",
    ],
    includes = ["include"],
    deps = [
        "//ecs",
    ],
)
