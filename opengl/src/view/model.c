#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/material/texture.h"
#include "shoveler/view/drawable.h"
#include "shoveler/view/model.h"
#include "shoveler/view/scene.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/view.h"

bool shovelerViewEntityAddModel(ShovelerViewEntity *entity, const ShovelerViewModelConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewModelComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateModelType());
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
		shovelerLogWarning("Trying to remove model from entity %lld which does not have a model, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentTypeName);
}
