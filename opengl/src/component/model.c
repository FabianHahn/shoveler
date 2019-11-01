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

static void *activateModelComponent(ShovelerComponent *component);
static void deactivateModelComponent(ShovelerComponent *component);
static void updateModelPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

ShovelerComponentType *shovelerComponentCreateModelType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewModelComponentTypeName, activateModelComponent, deactivateModelComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewModelPositionOptionKey, shovelerComponentTypeNamePosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateModelPositionDependency);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewModelDrawableOptionKey, shovelerViewDrawableComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewModelMaterialOptionKey, shovelerViewMaterialComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelRotationOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelScaleOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelVisibleOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelEmitterOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelCastsShadowOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelPolygonModeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerModel *shovelerComponentGetModel(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewModelComponentTypeName) == 0);

	return component->data;
}

static void *activateModelComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewModelPositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerComponent *drawableComponent = shovelerComponentGetDependency(component, shovelerViewModelDrawableOptionKey);
	assert(drawableComponent != NULL);
	ShovelerDrawable *drawable = shovelerComponentGetDrawable(drawableComponent);
	assert(drawable != NULL);

	ShovelerComponent *materialComponent = shovelerComponentGetDependency(component, shovelerViewModelMaterialOptionKey);
	assert(materialComponent != NULL);
	ShovelerMaterial *material = shovelerComponentGetMaterial(materialComponent);
	assert(material != NULL);

	ShovelerModel *model = shovelerModelCreate(drawable, material);
	model->translation = *positionCoordinates;
	model->rotation = shovelerComponentGetConfigurationValueVector3(component, shovelerViewModelRotationOptionKey);
	model->scale = shovelerComponentGetConfigurationValueVector3(component, shovelerViewModelScaleOptionKey);
	model->visible = shovelerComponentGetConfigurationValueBool(component, shovelerViewModelVisibleOptionKey);
	model->emitter = shovelerComponentGetConfigurationValueBool(component, shovelerViewModelEmitterOptionKey);
	model->castsShadow = shovelerComponentGetConfigurationValueBool(component, shovelerViewModelCastsShadowOptionKey);
	model->polygonMode = shovelerComponentGetConfigurationValueInt(component, shovelerViewModelPolygonModeOptionKey);
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

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewModelPositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	model->translation = *positionCoordinates;
	shovelerModelUpdateTransformation(model);
}
