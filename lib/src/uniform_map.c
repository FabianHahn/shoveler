#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free
#include <string.h> // strdup

#include "shoveler/uniform_map.h"

static void freeUniform(void *uniformPointer);

ShovelerUniformMap *shovelerUniformMapCreate()
{
	ShovelerUniformMap *uniformMap = malloc(sizeof(ShovelerUniformMap));
	uniformMap->uniforms = g_hash_table_new_full(g_str_hash, g_str_equal, free, freeUniform);
	return uniformMap;
}

bool shovelerUniformMapInsert(ShovelerUniformMap *uniformMap, const char *name, ShovelerUniform *uniform)
{
	if(g_hash_table_contains(uniformMap->uniforms, name)) {
		return false;
	}

	return g_hash_table_insert(uniformMap->uniforms, strdup(name), uniform);
}

bool shovelerUniformMapRemove(ShovelerUniformMap *uniformMap, const char *name)
{
	return g_hash_table_remove(uniformMap->uniforms, name);
}

void shovelerUniformMapFree(ShovelerUniformMap *uniformMap)
{
	if(uniformMap == NULL) {
		return;
	}

	g_hash_table_destroy(uniformMap->uniforms);
	free(uniformMap);
}

static void freeUniform(void *uniformPointer)
{
	shovelerUniformFree(uniformPointer);
}
