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

static const char *shovelerViewMaterialComponentTypeName = "material";
static const char *ShovelerComponentMaterialTypeOptionKey = "type";
static const char *shovelerViewMaterialTextureOptionKey = "texture";
static const char *shovelerViewMaterialTextureSamplerOptionKey = "texture_sampler";
static const char *shovelerViewMaterialTilemapOptionKey = "tilemap";
static const char *shovelerViewMaterialCanvasOptionKey = "canvas";
static const char *shovelerViewMaterialChunkOptionKey = "chunk";
static const char *shovelerViewMaterialColorOptionKey = "color";
static const char *shovelerViewMaterialCanvasRegionPositionOptionKey = "canvas_region_position";
static const char *shovelerViewMaterialCanvasRegionSizeOptionKey = "canvas_region_size";

ShovelerComponentType *shovelerComponentCreateMaterialType();
ShovelerMaterial *shovelerComponentGetMaterial(ShovelerComponent *component);

#endif
