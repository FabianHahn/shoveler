#ifndef SHOVELER_VIEW_MATERIAL_H
#define SHOVELER_VIEW_MATERIAL_H

#include <stdbool.h> // bool

#include <shoveler/material.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef enum {
	SHOVELER_VIEW_MATERIAL_TYPE_COLOR,
	SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE,
	SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE,
	SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP,
	SHOVELER_VIEW_MATERIAL_TYPE_CANVAS,
	SHOVELER_VIEW_MATERIAL_TYPE_CHUNK,
} ShovelerViewMaterialType;

typedef struct {
	ShovelerViewMaterialType type;
	long long int textureEntityId;
	long long int textureSamplerEntityId;
	long long int tilemapEntityId;
	long long int canvasEntityId;
	long long int chunkEntityId;
	ShovelerVector3 color;
	ShovelerVector2 canvasRegionPosition;
	ShovelerVector2 canvasRegionSize;
} ShovelerViewMaterialConfiguration;

static const char *shovelerViewMaterialComponentTypeName = "material";
static const char *shovelerViewMaterialTypeOptionKey = "type";
static const char *shovelerViewMaterialTextureOptionKey = "texture";
static const char *shovelerViewMaterialTextureSamplerOptionKey = "texture_sampler";
static const char *shovelerViewMaterialTilemapOptionKey = "tilemap";
static const char *shovelerViewMaterialCanvasOptionKey = "canvas";
static const char *shovelerViewMaterialChunkOptionKey = "chunk";
static const char *shovelerViewMaterialColorOptionKey = "color";
static const char *shovelerViewMaterialCanvasRegionPositionOptionKey = "canvas_region_position";
static const char *shovelerViewMaterialCanvasRegionSizeOptionKey = "canvas_region_size";

/** Adds a chunk material component to an entity, copying the supplied configuration. */
ShovelerComponent *shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration);
ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity);
/** Returns the current material configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetMaterialConfiguration(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration *outputConfiguration);
/** Updates a material component of an entity, copying the supplied configuration. */
bool shovelerViewEntityUpdateMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration);
bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetMaterialComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentTypeName);
}

#endif
