#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/texture.h"
#include "shoveler/view/drawable.h"
#include "shoveler/view/material.h"
#include "shoveler/view/model.h"
#include "shoveler/view/position.h"
#include "shoveler/view/scene.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/view.h"

static void *activateModelComponent(ShovelerComponent *component);
static void deactivateModelComponent(ShovelerComponent *component);
static void updateModelPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

bool shovelerViewEntityAddModel(ShovelerViewEntity *entity, const ShovelerViewModelConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewModelComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewModelComponentTypeName, activateModelComponent, deactivateModelComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewModelPositionOptionKey, shovelerViewPositionComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateModelPositionDependency);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewModelDrawableOptionKey, shovelerViewDrawableComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewModelMaterialOptionKey, shovelerViewMaterialComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelRotationOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelScaleOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelVisibleOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelEmitterOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelCastsShadowOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_BOOL, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewModelPolygonModeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewModelComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewModelPositionOptionKey, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewModelDrawableOptionKey, configuration->drawableEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewModelMaterialOptionKey, configuration->materialEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewModelRotationOptionKey, configuration->rotation);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewModelScaleOptionKey, configuration->scale);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewModelVisibleOptionKey, configuration->visible);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewModelEmitterOptionKey, configuration->emitter);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerViewModelCastsShadowOptionKey, configuration->castsShadow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewModelPolygonModeOptionKey, configuration->polygonMode);

	shovelerComponentActivate(component);
	return component;
}

ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetModelConfiguration(ShovelerViewEntity *entity, ShovelerViewModelConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelPositionOptionKey);
	outputConfiguration->drawableEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelDrawableOptionKey);
	outputConfiguration->materialEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelMaterialOptionKey);
	outputConfiguration->rotation = shovelerComponentGetConfigurationValueVector3(component, shovelerViewModelRotationOptionKey);
	outputConfiguration->scale = shovelerComponentGetConfigurationValueVector3(component, shovelerViewModelScaleOptionKey);
	outputConfiguration->visible = shovelerComponentGetConfigurationValueBool(component, shovelerViewModelVisibleOptionKey);
	outputConfiguration->emitter = shovelerComponentGetConfigurationValueBool(component, shovelerViewModelEmitterOptionKey);
	outputConfiguration->castsShadow = shovelerComponentGetConfigurationValueBool(component, shovelerViewModelCastsShadowOptionKey);
	outputConfiguration->polygonMode = shovelerComponentGetConfigurationValueInt(component, shovelerViewModelPolygonModeOptionKey);
	return true;
}

bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove model from entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentTypeName);
}

static void *activateModelComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasScene(component->entity->view));

	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	long long int drawableEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelDrawableOptionKey);
	ShovelerViewEntity *drawableEntity = shovelerViewGetEntity(component->entity->view, drawableEntityId);
	assert(drawableEntity != NULL);
	ShovelerDrawable *drawable = shovelerViewEntityGetDrawable(drawableEntity);
	assert(drawable != NULL);

	long long int materialEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelMaterialOptionKey);
	ShovelerViewEntity *materialEntity = shovelerViewGetEntity(component->entity->view, materialEntityId);
	assert(materialEntity != NULL);
	ShovelerMaterial *material = shovelerViewEntityGetMaterial(materialEntity);
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

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneAddModel(scene, model);

	return model;
}

static void deactivateModelComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasScene(component->entity->view));
	assert(shovelerViewHasShaderCache(component->entity->view));

	ShovelerModel *model = (ShovelerModel *) component->data;

	ShovelerShaderCache *shaderCache = shovelerViewGetShaderCache(component->entity->view);
	shovelerShaderCacheInvalidateModel(shaderCache, model);

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneRemoveModel(scene, model);
}

static void updateModelPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerModel *model = (ShovelerModel *) component->data;

	long long int positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewModelPositionOptionKey);
	ShovelerViewEntity *positionEntity = shovelerViewGetEntity(component->entity->view, positionEntityId);
	assert(positionEntity != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerViewEntityGetPositionCoordinates(positionEntity);
	assert(positionCoordinates != NULL);

	model->translation = *positionCoordinates;
	shovelerModelUpdateTransformation(model);
}
