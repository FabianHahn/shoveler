#ifndef SHOVELER_VIEW_SHADER_CACHE_H
#define SHOVELER_VIEW_SHADER_CACHE_H

#include <shoveler/shader_cache.h>
#include <shoveler/view.h>

static const char *shovelerViewShaderCacheTargetName = "shader_cache";

static inline void shovelerViewSetShaderCache(ShovelerView *view, ShovelerShaderCache *cache)
{
	shovelerViewSetTarget(view, shovelerViewShaderCacheTargetName, cache);
}

static inline ShovelerShaderCache *shovelerViewGetShaderCache(ShovelerView *view)
{
	return (ShovelerShaderCache *) shovelerViewGetTarget(view, shovelerViewShaderCacheTargetName);
}

static inline bool shovelerViewHasShaderCache(ShovelerView *view)
{
	return shovelerViewGetShaderCache(view) != NULL;
}

#endif
