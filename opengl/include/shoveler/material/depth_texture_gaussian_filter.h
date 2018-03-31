#ifndef SHOVELER_MATERIAL_DEPTH_TEXTURE_GAUSSIAN_FILTER_H
#define SHOVELER_MATERIAL_DEPTH_TEXTURE_GAUSSIAN_FILTER_H

#include <shoveler/material.h>
#include <shoveler/sampler.h>
#include <shoveler/texture.h>

ShovelerMaterial *shovelerMaterialDepthTextureGaussianFilterGaussianFilterCreate(ShovelerTexture **texturePointer, ShovelerSampler **samplerPointer, int width, int height);
void shovelerMaterialDepthTextureGaussianFilterEnableExponentialLifting(ShovelerMaterial *material, float liftExponentialFactor);
void shovelerMaterialDepthTextureGaussianFilterDisableExponentialLifting(ShovelerMaterial *material);
void shovelerMaterialDepthTextureGaussianFilterSetDirection(ShovelerMaterial *material, bool filterX, bool filterY);

#endif
