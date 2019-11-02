#include "shoveler/component/model.h"

#include <string.h> // strcmp

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
static void updateModelPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

ShovelerComponentType *shovelerComponentCreateModelType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdModel, activateModelComponent, deactivateModelComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentModelOptionKeyPosition, shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateModelPositionDependency);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentModelOptionKeyDrawable, shovelerComponentTypeIdDrawable, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentModelOptionKeyMaterial, shovelerComponentTypeIdMaterial, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentModelOptionKeyRotation, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentModelOptionKeyScale, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentModelOptionKeyVisible, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentModelOptionKeyEmitter, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentModelOptionKeyCastsShadow, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentModelOptionKeyPolygonMode, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerModel *shovelerComponentGetModel(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdModel) == 0);

	return component->data;
}

static void *activateModelComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerComponentModelOptionKeyPosition);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerComponent *drawableComponent = shovelerComponentGetDependency(component, shovelerComponentModelOptionKeyDrawable);
	assert(drawableComponent != NULL);
	ShovelerDrawable *drawable = shovelerComponentGetDrawable(drawableComponent);
	assert(drawable != NULL);

	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, shovelerComponentModelOptionKeyMaterial);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	ShovelerModel *model = shovelerModelCreate(drawable, material);
	model->translation = *positionCoordinates;
	model->rotation = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentModelOptionKeyRotation);
	model->scale = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentModelOptionKeyScale);
	model->visible = shovelerComponentGetConfigurationValueBool(component, shovelerComponentModelOptionKeyVisible);
	model->emitter = shovelerComponentGetConfigurationValueBool(component, shovelerComponentModelOptionKeyEmitter);
	model->castsShadow = shovelerComponentGetConfigurationValueBool(component, shovelerComponentModelOptionKeyCastsShadow);
	model->polygonMode = shovelerComponentGetConfigurationValueInt(component, shovelerComponentModelOptionKeyPolygonMode);
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

static void updateModelPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerModel *model = (ShovelerModel *) component->data;

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerComponentModelOptionKeyPosition);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	model->translation = *positionCoordinates;
	shovelerModelUpdateTransformation(model);
}
