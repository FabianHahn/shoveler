#ifndef SHOVELER_VIEW_MODEL_H
#define SHOVELER_VIEW_MODEL_H

#include <stdbool.h> // bool

#include <glib.h>

#include <shoveler/model.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

typedef struct {
	long long int drawableEntityId;
	long long int materialEntityId;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	bool visible;
	bool emitter;
	bool screenspace;
	bool castsShadow;
	GLuint polygonMode;
} ShovelerViewModelConfiguration;

static const char *shovelerViewModelComponentName = "model";

bool shovelerViewEntityAddModel(ShovelerViewEntity *entity, ShovelerViewModelConfiguration configuration);
ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity);
const ShovelerViewModelConfiguration *shovelerViewEntityGetModelConfiguration(ShovelerViewEntity *entity);
bool shovelerViewEntityUpdateModelDrawableEntityId(ShovelerViewEntity *entity, long long int drawableEntityId);
bool shovelerViewEntityUpdateModelMaterialEntityId(ShovelerViewEntity *entity, long long int materialEntityId);
bool shovelerViewEntityUpdateModelRotation(ShovelerViewEntity *entity, ShovelerVector3 rotation);
bool shovelerViewEntityUpdateModelScale(ShovelerViewEntity *entity, ShovelerVector3 scale);
bool shovelerViewEntityUpdateModelVisible(ShovelerViewEntity *entity, bool visible);
bool shovelerViewEntityUpdateModelEmitter(ShovelerViewEntity *entity, bool emitter);
bool shovelerViewEntityUpdateModelScreenspace(ShovelerViewEntity *entity, bool screenspace);
bool shovelerViewEntityUpdateModelPolygonMode(ShovelerViewEntity *entity, GLuint polygonMode);
bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity);

#endif
