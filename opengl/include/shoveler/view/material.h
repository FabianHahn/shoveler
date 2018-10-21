#ifndef SHOVELER_VIEW_MATERIAL_H
#define SHOVELER_VIEW_MATERIAL_H

#include <shoveler/material.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef enum {
	SHOVELER_VIEW_MATERIAL_TYPE_COLOR,
	SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE,
	SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE,
	SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP,
} ShovelerViewMaterialType;

typedef struct {
	ShovelerViewMaterialType type;
	ShovelerVector3 color;
	long long int textureEntityId;
	long long int tilemapEntityId;
} ShovelerViewMaterialConfiguration;

static const char *shovelerViewMaterialComponentName = "material";

bool shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration);
ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity);
const ShovelerViewMaterialConfiguration *shovelerViewEntityGetMaterialConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration);
bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity);

#endif
