#ifndef SHOVELER_COMPONENT_MATERIAL_H
#define SHOVELER_COMPONENT_MATERIAL_H

typedef struct ShovelerMaterialStruct ShovelerMaterial; // forward declaration: material.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

typedef enum {
	SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR,
	SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE,
	SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE,
	SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP,
	SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS,
	SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK,
} ShovelerComponentMaterialType;

extern const char *const shovelerComponentTypeIdMaterial;
static const char *shovelerComponentMaterialOptionKeyType = "type";
static const char *shovelerComponentMaterialOptionKeyTexture = "texture";
static const char *shovelerComponentMaterialOptionKeyTextureSampler = "texture_sampler";
static const char *shovelerComponentMaterialOptionKeyTilemap = "tilemap";
static const char *shovelerComponentMaterialOptionKeyCanvas = "canvas";
static const char *shovelerComponentMaterialOptionKeyChunk = "chunk";
static const char *shovelerComponentMaterialOptionKeyColor = "color";
static const char *shovelerComponentMaterialOptionKeyCanvasRegionPosition = "canvas_region_position";
static const char *shovelerComponentMaterialOptionKeyCanvasRegionSize = "canvas_region_size";

ShovelerComponentType *shovelerComponentCreateMaterialType();
ShovelerMaterial *shovelerComponentGetMaterial(ShovelerComponent *component);

#endif
