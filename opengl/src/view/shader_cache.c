#include "shoveler/view/shader_cache.h"

#include "shoveler/component/shader_cache.h"
#include "shoveler/shader_cache.h"
#include "shoveler/view.h"

bool shovelerViewSetShaderCache(ShovelerView *view, ShovelerShaderCache *shaderCache)
{
	return shovelerViewSetTarget(view, shovelerComponentViewTargetIdShaderCache, shaderCache);
}

ShovelerShaderCache *shovelerViewGetShaderCache(ShovelerView *view)
{
	return (ShovelerShaderCache *) shovelerViewGetTarget(view, shovelerComponentViewTargetIdShaderCache);
}
