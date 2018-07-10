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
	SHOVELER_VIEW_DRAWABLE_TYPE_CUBE,
	SHOVELER_VIEW_DRAWABLE_TYPE_QUAD,
	SHOVELER_VIEW_DRAWABLE_TYPE_POINT,
} ShovelerViewDrawableType;

typedef struct {
	ShovelerViewDrawableType type;
} ShovelerViewDrawableConfiguration;

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
	ShovelerViewDrawableConfiguration drawable;
	ShovelerViewMaterialConfiguration material;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	bool visible;
	bool emitter;
	bool screenspace;
	bool castsShadow;
	GLuint polygonMode;
} ShovelerViewModelConfiguration;

typedef struct {
	ShovelerModel *model;
	ShovelerViewComponentCallback *positionCallback;
} ShovelerViewModel;

static const char *shovelerViewModelComponentName = "model";

bool shovelerViewAddEntityModel(ShovelerView *view, long long int entityId, ShovelerViewModelConfiguration modelConfiguration);
bool shovelerViewUpdateEntityModelDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration drawableConfiguration);
bool shovelerViewUpdateEntityModelMaterial(ShovelerView *view, long long int entityId, ShovelerViewMaterialConfiguration materialConfiguration);
bool shovelerViewUpdateEntityModelRotation(ShovelerView *view, long long int entityId, ShovelerVector3 rotation);
bool shovelerViewUpdateEntityModelScale(ShovelerView *view, long long int entityId, ShovelerVector3 scale);
bool shovelerViewUpdateEntityModelVisible(ShovelerView *view, long long int entityId, bool visible);
bool shovelerViewUpdateEntityModelEmitter(ShovelerView *view, long long int entityId, bool emitter);
bool shovelerViewUpdateEntityModelScreenspace(ShovelerView *view, long long int entityId, bool screenspace);
bool shovelerViewUpdateEntityModelPolygonMode(ShovelerView *view, long long int entityId, GLuint polygonMode);
bool shovelerViewRemoveEntityModel(ShovelerView *view, long long int entityId);

#endif
