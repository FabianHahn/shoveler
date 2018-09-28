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

bool shovelerViewAddEntityModel(ShovelerView *view, long long int entityId, ShovelerViewModelConfiguration modelConfiguration);
ShovelerModel *shovelerViewGetEntityModel(ShovelerView *view, long long int entityId);
ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity);
bool shovelerViewUpdateEntityModelDrawableEntityId(ShovelerView *view, long long int entityId, long long int drawableEntityId);
bool shovelerViewUpdateEntityModelMaterial(ShovelerView *view, long long int entityId, ShovelerViewMaterialConfiguration materialConfiguration);
bool shovelerViewUpdateEntityModelRotation(ShovelerView *view, long long int entityId, ShovelerVector3 rotation);
bool shovelerViewUpdateEntityModelScale(ShovelerView *view, long long int entityId, ShovelerVector3 scale);
bool shovelerViewUpdateEntityModelVisible(ShovelerView *view, long long int entityId, bool visible);
bool shovelerViewUpdateEntityModelEmitter(ShovelerView *view, long long int entityId, bool emitter);
bool shovelerViewUpdateEntityModelScreenspace(ShovelerView *view, long long int entityId, bool screenspace);
bool shovelerViewUpdateEntityModelPolygonMode(ShovelerView *view, long long int entityId, GLuint polygonMode);
bool shovelerViewRemoveEntityModel(ShovelerView *view, long long int entityId);

#endif
