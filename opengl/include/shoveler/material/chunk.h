#ifndef SHOVELER_MATERIAL_CHUNK_H
#define SHOVELER_MATERIAL_CHUNK_H

#include <stdbool.h> // bool

#include <shoveler/chunk.h>
#include <shoveler/material.h>

ShovelerMaterial *shovelerMaterialChunkCreate(ShovelerChunk *chunk, bool manageChunk);

#endif
