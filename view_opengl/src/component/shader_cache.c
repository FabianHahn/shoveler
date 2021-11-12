#include "shoveler/component/shader_cache.h"

#include "shoveler/component.h"

const char *const shovelerComponentViewTargetIdShaderCache = "shader_cache";

ShovelerShaderCache *shovelerComponentGetViewShaderCache(ShovelerComponent *component)
{
	return (ShovelerShaderCache *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetIdShaderCache);
}
