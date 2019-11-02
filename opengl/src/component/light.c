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
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewLightComponentTypeName, activateLightComponent, deactivateLightComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewLightPositionOptionKey, shovelerComponentTypeNamePosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, updateLightPositionDependency);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightWidthOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightHeightOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightSamplesOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightAmbientFactorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightExponentialFactorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewLightColorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerLight *shovelerComponentGetLight(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewLightComponentTypeName) == 0);

	return component->data;
}


static void *activateLightComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewLightPositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	ShovelerComponentLightType type = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightTypeOptionKey);
	ShovelerLight *light;
	switch(type) {
		case SHOVELER_COMPONENT_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return NULL;
		case SHOVELER_COMPONENT_LIGHT_TYPE_POINT: {
			ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);

			int width = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightWidthOptionKey);
			int height = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightHeightOptionKey);
			int samples = shovelerComponentGetConfigurationValueInt(component, shovelerViewLightSamplesOptionKey);
			float ambientFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerViewLightAmbientFactorOptionKey);
			float exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, shovelerViewLightExponentialFactorOptionKey);
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewLightColorOptionKey);

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

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, shovelerViewLightPositionOptionKey);
	assert(positionComponent != NULL);
	const ShovelerVector3 *positionCoordinates = shovelerComponentGetPositionCoordinates(positionComponent);
	assert(positionCoordinates != NULL);

	shovelerLightUpdatePosition(light, *positionCoordinates);
}
