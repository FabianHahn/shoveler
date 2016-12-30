#ifndef SHOVELER_MATERIAL_SCREENSPACE_TEXTURE_H
#define SHOVELER_MATERIAL_SCREENSPACE_TEXTURE_H

#include <stdbool.h> // bool

#include "material.h"
#include "sampler.h"
#include "texture.h"

ShovelerMaterial *shovelerMaterialScreenspaceTextureCreate(ShovelerTexture *texture, bool manageTexture, bool depthTexture, ShovelerSampler *sampler, bool manageSampler);

#endif
