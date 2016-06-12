#ifndef SHOVELER_UNIFORM_MAP_H
#define SHOVELER_UNIFORM_MAP_H

#include <glib.h>

#include "uniform.h"

typedef struct {
	GHashTable *uniforms;
} ShovelerUniformMap;

ShovelerUniformMap *shovelerUniformMapCreate();
bool shovelerUniformMapInsert(ShovelerUniformMap *uniformMap, const char *name, ShovelerUniform *uniform);
bool shovelerUniformMapRemove(ShovelerUniformMap *uniformMap, const char *name);
void shovelerUniformMapFree(ShovelerUniformMap *uniformMap);

#endif
