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
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeNameModel)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateModelType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeNameModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentModelOptionKeyPosition, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentModelOptionKeyDrawable, configuration->drawableEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentModelOptionKeyMaterial, configuration->materialEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentModelOptionKeyRotation, configuration->rotation);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentModelOptionKeyScale, configuration->scale);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentModelOptionKeyVisible, configuration->visible);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentModelOptionKeyEmitter, configuration->emitter);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, shovelerComponentModelOptionKeyCastsShadow, configuration->castsShadow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentModelOptionKeyPolygonMode, configuration->polygonMode);

	shovelerComponentActivate(component);
	return component;
}

ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameModel);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetModelConfiguration(ShovelerViewEntity *entity, ShovelerViewModelConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameModel);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentModelOptionKeyPosition);
	outputConfiguration->drawableEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentModelOptionKeyDrawable);
	outputConfiguration->materialEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentModelOptionKeyMaterial);
	outputConfiguration->rotation = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentModelOptionKeyRotation);
	outputConfiguration->scale = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentModelOptionKeyScale);
	outputConfiguration->visible = shovelerComponentGetConfigurationValueBool(component, shovelerComponentModelOptionKeyVisible);
	outputConfiguration->emitter = shovelerComponentGetConfigurationValueBool(component, shovelerComponentModelOptionKeyEmitter);
	outputConfiguration->castsShadow = shovelerComponentGetConfigurationValueBool(component, shovelerComponentModelOptionKeyCastsShadow);
	outputConfiguration->polygonMode = shovelerComponentGetConfigurationValueInt(component, shovelerComponentModelOptionKeyPolygonMode);
	return true;
}

bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeNameModel);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove model from entity %lld which does not have a model, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeNameModel);
}
