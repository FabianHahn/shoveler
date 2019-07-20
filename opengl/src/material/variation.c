#include <stdlib.h> // malloc, free

#include "shoveler/material/variation.h"
#include "shoveler/shader.h"
#include "shoveler/shader_cache.h"
#include "shoveler/uniform_map.h"

typedef struct {
	ShovelerMaterial *material;
	ShovelerMaterial *delegate;
} Variation;

static int attachUniforms(ShovelerMaterial *variationMaterial, ShovelerShader *shader, void *userData);
static void freeVariation(ShovelerMaterial *variationMaterial);

ShovelerMaterial *shovelerMaterialVariationCreate(ShovelerShaderCache *shaderCache, ShovelerMaterial *delegate)
{
	Variation *variation = malloc(sizeof(Variation));
	variation->material = shovelerMaterialCreateUnmanaged(shaderCache, delegate->screenspace, delegate->program);
	variation->material->data = variation;
	variation->material->attachUniforms = attachUniforms;
	variation->material->freeData = freeVariation;
	variation->delegate = delegate;

	return variation->material;
}

static int attachUniforms(ShovelerMaterial *variationMaterial, ShovelerShader *shader, void *userData)
{
	Variation *variation = variationMaterial->data;

	int attached = 0;
	attached += shovelerMaterialAttachUniforms(variation->delegate, shader, userData);
	attached += shovelerUniformMapAttach(variationMaterial->uniforms, shader);
	return attached;
}

static void freeVariation(ShovelerMaterial *variationMaterial)
{
	Variation *variation = variationMaterial->data;
	free(variation);
}
