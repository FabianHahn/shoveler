#ifndef SHOVELER_MATERIAL_DEPTH_TEXTURE_GAUSSIAN_FILTER_H
#define SHOVELER_MATERIAL_DEPTH_TEXTURE_GAUSSIAN_FILTER_H

#include "material.h"
#include "sampler.h"
#include "texture.h"

ShovelerMaterial *shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(ShovelerTexture *texture, bool manageTexture, int width, int height, bool filterX, bool filterY);
void shovelerMaterialDepthTextureGaussianFilterSetDirection(ShovelerMaterial *material, bool filterX, bool filterY);

#endif
