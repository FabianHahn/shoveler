package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ecs",
    srcs = [
        "src/component.c",
        "src/component_field.c",
        "src/component_system.c",
        "src/component_type.c",
        "src/entity_id_allocator.c",
        "src/schema.c",
        "src/system.c",
        "src/world.c",
        "src/world_dependency_graph.c",
    ],
    hdrs = [
        "include/shoveler/component.h",
        "include/shoveler/component_field.h",
        "include/shoveler/component_system.h",
        "include/shoveler/component_type.h",
        "include/shoveler/entity_component_id.h",
        "include/shoveler/entity_id_allocator.h",
        "include/shoveler/schema.h",
        "include/shoveler/system.h",
        "include/shoveler/world.h",
        "include/shoveler/world_dependency_graph.h",
    ],
    includes = ["include"],
    deps = [
        "//base",
    ],
)

cc_test(
    name = "ecs_tests",
    srcs = [
        "src/component_test.cpp",
        "src/test.cpp",
        "src/test_component_types.h",
        "src/world_test.cpp",
    ],
    linkstatic = True,
    deps = [
        ":ecs",
        "@googletest//:gtest",
    ],
)
