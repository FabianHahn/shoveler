#include "shoveler/component/light.h"

#include <string.h> // strcmp

#include "shoveler/component/position.h"
#include "shoveler/component/scene.h"
#include "shoveler/component/shader_cache.h"
#include "shoveler/light/point.h"
#include "shoveler/light/spot.h"
#include "shoveler/component.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"

static void *activateLightComponent(ShovelerComponent *component);
static void deactivateLightComponent(ShovelerComponent *component);
static void updateLightPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

ShovelerComponentType *shovelerComponentCreateLightType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeNameLight, activateLightComponent, deactivateLightComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentLightOptionKeyPosition, shovelerComponentTypeNamePosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateLightPositionDependency);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeyType, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeyWidth, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeyHeight, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeySamples, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeyAmbientFactor, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeyExponentialFactor, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentLightOptionKeyColor, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerLight *shovelerComponentGetLight(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerComponentTypeNameLight) == 0);

	return component->data;
}


static void *activateLightComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerComponentLightOptionKeyPosition);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerComponentLightType type = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeyType);
	ShovelerLight *light;
	switch(type) {
		case SHOVELER_COMPONENT_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return NULL;
		case SHOVELER_COMPONENT_LIGHT_TYPE_POINT: {
			ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);

			int width = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeyWidth);
			int height = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeyHeight);
			int samples = shovelerComponentGetConfigurationValueInt(component, shovelerComponentLightOptionKeySamples);
			float ambientFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerComponentLightOptionKeyAmbientFactor);
			float exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerComponentLightOptionKeyExponentialFactor);
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentLightOptionKeyColor);

			light = shovelerLightPointCreate(shaderCache, *positionCoordinates, width, height, samples, ambientFactor, exponentialFactor, color);
		} break;
		default:
			shovelerLogWarning("Trying to create light with unknown light type %d, ignoring.", type);
			return NULL;
	}

	ShovelerScene *scene = shovelerComponentGetViewScene(component);
	shovelerSceneAddLight(scene, light);
	return light;
}

static void deactivateLightComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerLight *light = (ShovelerLight *) component->data;

	ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);
	shovelerShaderCacheInvalidateLight(shaderCache, light);

	ShovelerScene *scene = shovelerComponentGetViewScene(component);
	shovelerSceneRemoveLight(scene, light);
}

static void updateLightPositionDependency(ShovelerComponent *component, ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerLight *light = (ShovelerLight *) component->data;

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerComponentLightOptionKeyPosition);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	shovelerLightUpdatePosition(light, *positionCoordinates);
}
