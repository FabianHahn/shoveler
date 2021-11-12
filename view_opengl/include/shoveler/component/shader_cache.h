#ifndef SHOVELER_COMPONENT_SHADER_CACHE_H
#define SHOVELER_COMPONENT_SHADER_CACHE_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerShaderCacheStruct ShovelerShaderCache; // forward declaration: shader_cache.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentViewTargetIdShaderCache;

ShovelerShaderCache *shovelerComponentGetViewShaderCache(ShovelerComponent *component);

static inline bool shovelerComponentHasViewShaderCache(ShovelerComponent *component)
{
	return shovelerComponentGetViewShaderCache(component) != NULL;
}

#endif
