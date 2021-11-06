#include "shoveler/world_dependency_graph.h"

#include "shoveler/component.h"
#include "shoveler/entity_component_id.h"
#include "shoveler/file.h"
#include "shoveler/world.h"

GString* ShovelerWorldCreateDependencyGraph(ShovelerWorld* world) {
  GString* graph = g_string_new("");
  g_string_append(graph, "digraph G {\n");
  g_string_append(graph, "	rankdir = \"LR\";\n");

  GHashTableIter entityIter;
  long long int* entityIdPointer;
  ShovelerWorldEntity* entity;
  g_hash_table_iter_init(&entityIter, world->entities);
  while (g_hash_table_iter_next(&entityIter, (gpointer*) &entityIdPointer, (gpointer*) &entity)) {
    long long int entityId = *entityIdPointer;

    g_string_append_printf(graph, "	subgraph cluster_entity%lld {\n", entityId);

    if (entity->label != NULL) {
      g_string_append_printf(graph, "		label = \"%s %lld\";\n", entity->label, entityId);
    } else {
      g_string_append_printf(graph, "		label = \"%lld\";\n", entityId);
    }

    GHashTableIter componentIter;
    const char* componentTypeId;
    ShovelerComponent* component;
    g_hash_table_iter_init(&componentIter, entity->components);
    while (g_hash_table_iter_next(
        &componentIter, (gpointer*) &componentTypeId, (gpointer*) &component)) {
      const char* color = shovelerComponentIsActive(component) ? "green" : "red";
      g_string_append_printf(
          graph,
          "		entity%lld_%s [label = <<font color='%s'>%s</font>>];\n",
          entityId,
          componentTypeId,
          color,
          componentTypeId);
    }

    g_string_append(graph, "	}\n");

    g_hash_table_iter_init(&componentIter, entity->components);
    while (g_hash_table_iter_next(
        &componentIter, (gpointer*) &componentTypeId, (gpointer*) &component)) {
      ShovelerEntityComponentId dependencySource;
      dependencySource.entityId = entity->id;
      dependencySource.componentTypeId = (char*) componentTypeId; // won't be modified

      GArray* dependencies = g_hash_table_lookup(world->dependencies, &dependencySource);
      if (dependencies != NULL) {
        for (int i = 0; i < dependencies->len; i++) {
          const ShovelerEntityComponentId* dependencyTarget =
              &g_array_index(dependencies, ShovelerEntityComponentId, i);
          g_string_append_printf(
              graph,
              "	entity%lld_%s -> entity%lld_%s;\n",
              entityId,
              componentTypeId,
              dependencyTarget->entityId,
              dependencyTarget->componentTypeId);
        }
      }
    }
  }

  g_string_append(graph, "}\n");
  return graph;
}

bool shovelerWorldDependencyGraphWrite(ShovelerWorld* world, const char* filename) {
  GString* graph = ShovelerWorldCreateDependencyGraph(world);
  bool success = shovelerFileWriteString(filename, graph->str);
  g_string_free(graph, true);
  return success;
}
