#ifndef SHOVELER_WORLD_DEPENDENCY_GRAPH_H
#define SHOVELER_WORLD_DEPENDENCY_GRAPH_H

#include <glib.h>

typedef struct ShovelerWorldStruct ShovelerWorld;

GString *shovelerWorldDependencyGraphCreate(ShovelerWorld *world);
bool shovelerWorldDependencyGraphWrite(ShovelerWorld *world, const char *filename);

#endif
