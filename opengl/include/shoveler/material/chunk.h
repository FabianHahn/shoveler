#ifndef SHOVELER_MATERIAL_CHUNK_H
#define SHOVELER_MATERIAL_CHUNK_H

#include <stdbool.h> // bool

#include <shoveler/chunk.h>
#include <shoveler/material.h>

struct ShovelerShaderCacheStruct; // forward declaration: shader_cache.h

ShovelerMaterial *shovelerMaterialChunkCreate(struct ShovelerShaderCacheStruct *shaderCache, bool screenspace);
void shovelerMaterialChunkSetActive(ShovelerMaterial *material, ShovelerChunk *chunk);

#endif
