#ifndef SHOVELER_MATERIAL_TEXTURE_H
#define SHOVELER_MATERIAL_TEXTURE_H

#include "material.h"
#include "sampler.h"
#include "texture.h"

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
