#ifndef SHOVELER_FILTER_DEPTH_TEXTURE_GAUSSIAN_H
#define SHOVELER_FILTER_DEPTH_TEXTURE_GAUSSIAN_H

#include <shoveler/filter.h>

ShovelerFilter *shovelerFilterDepthTextureGaussianCreate(int width, int height, GLsizei samples, float exponentialFactor);

#endif
