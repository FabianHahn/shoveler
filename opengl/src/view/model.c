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
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdModel)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateModelType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, configuration->positionEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, configuration->drawableEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, configuration->materialEntityId);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, configuration->rotation);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, configuration->scale);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, configuration->visible);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, configuration->emitter);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, configuration->castsShadow);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, configuration->polygonMode);

	shovelerComponentActivate(component);
	return component;
}

ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdModel);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetModelConfiguration(ShovelerViewEntity *entity, ShovelerViewModelConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdModel);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->positionEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION);
	outputConfiguration->drawableEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE);
	outputConfiguration->materialEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL);
	outputConfiguration->rotation = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION);
	outputConfiguration->scale = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE);
	outputConfiguration->visible = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE);
	outputConfiguration->emitter = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER);
	outputConfiguration->castsShadow = shovelerComponentGetConfigurationValueBool(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW);
	outputConfiguration->polygonMode = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE);
	return true;
}

bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdModel);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove model from entity %lld which does not have a model, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdModel);
}
