#ifndef SHOVELER_UNIFORM_MAP_H
#define SHOVELER_UNIFORM_MAP_H

#include <glib.h>

#include <shoveler/uniform.h>

struct ShovelerShaderStruct; // forward declaration: shader.h

typedef struct ShovelerUniformMapStruct {
	GHashTable *uniforms;
} ShovelerUniformMap;

ShovelerUniformMap *shovelerUniformMapCreate();
bool shovelerUniformMapInsert(ShovelerUniformMap *uniformMap, const char *name, ShovelerUniform *uniform);
bool shovelerUniformMapRemove(ShovelerUniformMap *uniformMap, const char *name);
int shovelerUniformMapAttach(ShovelerUniformMap *uniformMap, struct ShovelerShaderStruct *shader);
void shovelerUniformMapFree(ShovelerUniformMap *uniformMap);

#endif
