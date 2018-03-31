#ifndef SHOVELER_MATERIAL_TEXTURE_H
#define SHOVELER_MATERIAL_TEXTURE_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

ShovelerMaterial *shovelerMaterialTextureCreate(ShovelerTexture *texture, bool manageTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
