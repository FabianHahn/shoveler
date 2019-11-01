#include "shoveler/component/shader_cache.h"

#include "shoveler/component.h"

ShovelerShaderCache *shovelerComponentGetViewShaderCache(ShovelerComponent *component)
{
	return (ShovelerShaderCache *) shovelerComponentGetViewTarget(component, shovelerComponentViewTargetNameShaderCache);
}
