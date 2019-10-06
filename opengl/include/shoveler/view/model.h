#ifndef SHOVELER_VIEW_MODEL_H
#define SHOVELER_VIEW_MODEL_H

#include <stdbool.h> // bool

#include <shoveler/model.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

typedef struct {
	long long int positionEntityId;
	long long int drawableEntityId;
	long long int materialEntityId;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	bool visible;
	bool emitter;
	bool castsShadow;
	GLuint polygonMode;
} ShovelerViewModelConfiguration;

static const char *shovelerViewModelComponentTypeName = "model";
static const char *shovelerViewModelPositionOptionKey = "position";
static const char *shovelerViewModelDrawableOptionKey = "drawable";
static const char *shovelerViewModelMaterialOptionKey = "material";
static const char *shovelerViewModelRotationOptionKey = "rotation";
static const char *shovelerViewModelScaleOptionKey = "scale";
static const char *shovelerViewModelVisibleOptionKey = "visible";
static const char *shovelerViewModelEmitterOptionKey = "emitter";
static const char *shovelerViewModelCastsShadowOptionKey = "castsShadow";
static const char *shovelerViewModelPolygonModeOptionKey = "polygonMode";

/** Adds a model component to an entity, copying the supplied configuration. */
bool shovelerViewEntityAddModel(ShovelerViewEntity *entity, const ShovelerViewModelConfiguration *configuration);
ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity);
/** Returns the current model configuration, retaining ownership over returned fields. */
bool shovelerViewEntityGetModelConfiguration(ShovelerViewEntity *entity, ShovelerViewModelConfiguration *outputConfiguration);
bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetModelComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerViewModelComponentTypeName);
}

#endif
