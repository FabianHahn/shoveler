#include <assert.h> // assert
#include <stdlib.h> // malloc free
#include <shoveler/view.h>

#include "shoveler/light/point.h"
#include "shoveler/view/light.h"
#include "shoveler/view/position.h"
#include "shoveler/view/scene.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/scene.h"
#include "shoveler/view.h"

typedef struct {
	ShovelerLight *light;
	ShovelerViewComponentCallback *positionCallback;
} LightComponentData;

static bool activateLightComponent(ShovelerComponent *component, void *lightComponentDataPointer);
static void deactivateLightComponent(ShovelerComponent *component, void *lightComponentDataPointer);
static void positionCallback(ShovelerComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *lightComponentDataPointer);

ShovelerComponent *shovelerViewEntityAddLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *lightConfiguration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewLightComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewLightComponentTypeName, activateDrawableComponent, deactivateDrawableComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewDrawableTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewDrawableTilesWidthOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewDrawableTilesHeightOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_UINT, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewDrawableComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewDrawableTypeOptionKey, configuration->type);
	if(configuration->type == SHOVELER_VIEW_DRAWABLE_TYPE_TILES) {
		shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewDrawableTilesWidthOptionKey, configuration->tilesWidth);
		shovelerComponentUpdateCanonicalConfigurationOptionUint(component, shovelerViewDrawableTilesHeightOptionKey, configuration->tilesHeight);
	}

	shovelerComponentActivate(component);
	return component;


	assert(shovelerViewHasScene(entity->view));

	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add light to entity %lld which already has a light, ignoring.", entity->entityId);
		return false;
	}

	LightComponentData *lightComponentData = malloc(sizeof(LightComponentData));
	lightComponentData->configuration = lightConfiguration;
	lightComponentData->light = NULL;
	lightComponentData->positionCallback = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewLightComponentTypeName, lightComponentData, activateLightComponent, deactivateLightComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

bool shovelerViewEntityGetLightConfiguration(ShovelerViewEntity *entity, ShovelerViewLightConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	LightComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateLight(ShovelerViewEntity *entity, const ShovelerViewLightConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update light of entity %lld which does not have a light, ignoring.", entity->entityId);
		return false;
	}

	LightComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);
	componentData->configuration = configuration;
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveLight(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewLightComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove light from entity %lld which does not have a light, ignoring.", entity->entityId);
		return false;
	}
	LightComponentData *lightComponentData = component->data;

	ShovelerScene *scene = shovelerViewGetScene(entity->view);
	shovelerSceneRemoveLight(scene, lightComponentData->light);

	return shovelerViewEntityRemoveComponent(entity, shovelerViewLightComponentTypeName);
}

static void positionCallback(ShovelerComponent *positionComponent, ShovelerComponentCallbackType callbackType, void *lightComponentDataPointer)
{
	LightComponentData *lightComponentData = lightComponentDataPointer;

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(positionComponent->entity);
	shovelerLightUpdatePosition(lightComponentData->light, shovelerVector3(position->x, position->y, position->z));
}

static bool activateComponent(ShovelerComponent *component, void *lightComponentDataPointer)
{
	LightComponentData *lightComponentData = lightComponentDataPointer;
	assert(shovelerViewHasScene(component->entity->view));
	assert(shovelerViewHasShaderCache(component->entity->view));

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(component->entity);

	switch(lightComponentData->configuration.type) {
		case SHOVELER_VIEW_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return false;
		case SHOVELER_VIEW_LIGHT_TYPE_POINT: {
			ShovelerShaderCache *shaderCache = shovelerViewGetShaderCache(component->entity->view);
			lightComponentData->light = shovelerLightPointCreate(shaderCache, shovelerVector3(position->x, position->y, position->z), lightComponentData->configuration.width, lightComponentData->configuration.height, lightComponentData->configuration.samples, lightComponentData->configuration.ambientFactor, lightComponentData->configuration.exponentialFactor, lightComponentData->configuration.color);
		} break;
		default:
			shovelerLogWarning("Trying to create light with unknown light type %d, ignoring.", lightComponentData->configuration.type);
			return false;
	}

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneAddLight(scene, lightComponentData->light);

	lightComponentData->positionCallback = shovelerViewEntityAddCallback(component->entity, shovelerViewPositionComponentName, &positionCallback, lightComponentData);
	return true;
}

static void deactivateComponent(ShovelerComponent *component, void *lightComponentDataPointer)
{
	LightComponentData *lightComponentData = lightComponentDataPointer;
	assert(shovelerViewHasScene(component->entity->view));
	assert(shovelerViewHasShaderCache(component->entity->view));

	ShovelerShaderCache *shaderCache = shovelerViewGetShaderCache(component->entity->view);
	shovelerShaderCacheInvalidateLight(shaderCache, lightComponentData->light);

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneRemoveLight(scene, lightComponentData->light);
	lightComponentData->light = NULL;

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, lightComponentData->positionCallback);
	lightComponentData->positionCallback = NULL;
}

static void freeComponent(ShovelerComponent *component, void *lightComponentDataPointer)
{
	assert(shovelerViewHasScene(component->entity->view));

	LightComponentData *lightComponentData = component->data;

	ShovelerLight *light = lightComponentData->light;

	if(light != NULL) {
		ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
		shovelerSceneRemoveLight(scene, light);
	}

	if(lightComponentData != NULL) {
		shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, lightComponentData->positionCallback);
	}

	free(lightComponentData);
}
