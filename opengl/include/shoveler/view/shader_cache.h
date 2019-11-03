#ifndef SHOVELER_VIEW_SHADER_CACHE_H
#define SHOVELER_VIEW_SHADER_CACHE_H

#include <shoveler/shader_cache.h>
#include <shoveler/view.h>

bool shovelerViewSetShaderCache(ShovelerView *view, ShovelerShaderCache *shaderCache);
ShovelerShaderCache *shovelerViewGetShaderCache(ShovelerView *view);

static inline bool shovelerViewHasShaderCache(ShovelerView *view)
{
	return shovelerViewGetShaderCache(view) != NULL;
}

#endif
