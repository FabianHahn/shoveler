#ifndef SHOVELER_MATERIAL_VARIATION_H
#define SHOVELER_MATERIAL_VARIATION_H

#include <shoveler/material.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

/** creates a variation of another material that can apply further uniform overrides */
ShovelerMaterial *shovelerMaterialVariationCreate(struct ShovelerShaderCacheStruct *shaderCache, ShovelerMaterial *delegate);

#endif
