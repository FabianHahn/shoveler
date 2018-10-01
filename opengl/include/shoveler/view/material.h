#ifndef SHOVELER_VIEW_MATERIAL_H
#define SHOVELER_VIEW_MATERIAL_H

#include <shoveler/material.h>
#include <shoveler/view.h>
#include <shoveler/types.h>

typedef enum {
	SHOVELER_VIEW_MATERIAL_TYPE_COLOR,
	SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE,
	SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE,
} ShovelerViewMaterialType;

typedef struct {
	ShovelerViewMaterialType type;
	ShovelerVector3 color;
	const char *texture;
} ShovelerViewMaterialConfiguration;

static const char *shovelerViewMaterialComponentName = "material";

bool shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration);
ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateMaterialConfiguration(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration);
bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity);

#endif
