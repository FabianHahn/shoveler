#ifndef SHOVELER_VIEW_MODEL_H
#define SHOVELER_VIEW_MODEL_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/drawable.h>
#include <shoveler/material.h>
#include <shoveler/model.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

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

typedef struct {
	long long int drawableEntityId;
	ShovelerViewMaterialConfiguration material;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	bool visible;
	bool emitter;
	bool screenspace;
	bool castsShadow;
	GLuint polygonMode;
} ShovelerViewModelConfiguration;

static const char *shovelerViewModelComponentName = "model";

bool shovelerViewEntityAddModel(ShovelerViewEntity *entity, ShovelerViewModelConfiguration modelConfiguration);
ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateModelDrawableEntityId(ShovelerViewEntity *entity, long long int drawableEntityId);
bool shovelerViewEntityUpdateModelMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration materialConfiguration);
bool shovelerViewEntityUpdateModelRotation(ShovelerViewEntity *entity, ShovelerVector3 rotation);
bool shovelerViewEntityUpdateModelScale(ShovelerViewEntity *entity, ShovelerVector3 scale);
bool shovelerViewEntityUpdateModelVisible(ShovelerViewEntity *entity, bool visible);
bool shovelerViewEntityUpdateModelEmitter(ShovelerViewEntity *entity, bool emitter);
bool shovelerViewEntityUpdateModelScreenspace(ShovelerViewEntity *entity, bool screenspace);
bool shovelerViewEntityUpdateModelPolygonMode(ShovelerViewEntity *entity, GLuint polygonMode);
bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity);

#endif
