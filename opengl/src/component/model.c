#include "shoveler/component/model.h"

#include "shoveler/component/drawable.h"
#include "shoveler/component/material.h"
#include "shoveler/component/position.h"
#include "shoveler/component/scene.h"
#include "shoveler/component/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/model.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"

const char *const shovelerComponentTypeIdModel = "model";

static void *activateModelComponent(ShovelerComponent *component);
static void deactivateModelComponent(ShovelerComponent *component);
static void updateModelPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

ShovelerComponentType *shovelerComponentCreateModelType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[9];
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION] = shovelerComponentTypeConfigurationOptionDependency("position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateModelPositionDependency);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE] = shovelerComponentTypeConfigurationOptionDependency("drawable", shovelerComponentTypeIdDrawable, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL] = shovelerComponentTypeConfigurationOptionDependency("material", shovelerComponentTypeIdMaterial, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION] = shovelerComponentTypeConfigurationOption("rotation", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE] = shovelerComponentTypeConfigurationOption("scale", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE] = shovelerComponentTypeConfigurationOption("visible", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER] = shovelerComponentTypeConfigurationOption("emitter", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW] = shovelerComponentTypeConfigurationOption("casts_shadow", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE] = shovelerComponentTypeConfigurationOption("polygon_mode", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdModel, activateModelComponent, deactivateModelComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerModel *shovelerComponentGetModel(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdModel);

	return component->data;
}

static void *activateModelComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerComponent *drawableComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE);
	assert(drawableComponent != NULL);
	ShovelerDrawable *drawable = shovelerComponentGetDrawable(drawableComponent);
	assert(drawable != NULL);

	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	ShovelerModel *model = shovelerModelCreate(drawable, material);
	model->translation = *positionCoordinates;
	model->rotation = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION);
	model->scale = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE);
	model->visible = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE);
	model->emitter = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER);
	model->castsShadow = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW);
	model->polygonMode = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE);
	shovelerModelUpdateTransformation(model);

	ShovelerScene *scene = shovelerComponentGetViewScene(component);
	shovelerSceneAddModel(scene, model);

	return model;
}

static void deactivateModelComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerModel *model = (ShovelerModel *) component->data;

	ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);
	shovelerShaderCacheInvalidateModel(shaderCache, model);

	ShovelerScene *scene = shovelerComponentGetViewScene(component);
	shovelerSceneRemoveModel(scene, model);
}

static void updateModelPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerModel *model = (ShovelerModel *) component->data;

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	model->translation = *positionCoordinates;
	shovelerModelUpdateTransformation(model);
}
