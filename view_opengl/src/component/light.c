#include "shoveler/component/light.h"

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

const char *const shovelerComponentTypeIdLight = "light";

static void *activateLightComponent(ShovelerComponent *component);
static void deactivateLightComponent(ShovelerComponent *component);
static bool liveUpdateLightPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent);

ShovelerComponentType *shovelerComponentCreateLightType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[8];
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION] = shovelerComponentTypeConfigurationOptionDependency("position", shovelerComponentTypeIdPosition, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, liveUpdateLightPositionDependency);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH] = shovelerComponentTypeConfigurationOption("width", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT] = shovelerComponentTypeConfigurationOption("height", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES] = shovelerComponentTypeConfigurationOption("samples", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR] = shovelerComponentTypeConfigurationOption("ambient_factor", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR] = shovelerComponentTypeConfigurationOption("exponential_factor", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_FLOAT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR] = shovelerComponentTypeConfigurationOption("tiles_height", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ false, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdLight, activateLightComponent, /* update */ NULL, deactivateLightComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerLight *shovelerComponentGetLight(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdLight);

	return component->data;
}


static void *activateLightComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewScene(component));
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *position = shovelerComponentGetPosition(positionComponent);

	ShovelerComponentLightType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE);
	ShovelerLight *light;
	switch(type) {
		case SHOVELER_COMPONENT_LIGHT_TYPE_SPOT:
			shovelerLogWarning("Trying to create light with unsupported spot type, ignoring.");
			return NULL;
		case SHOVELER_COMPONENT_LIGHT_TYPE_POINT: {
			ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);

			int width = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH);
			int height = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT);
			int samples = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES);
			float ambientFactor = shovelerComponentGetConfigurationValueFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR);
			float exponentialFactor = shovelerComponentGetConfigurationValueFloat(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR);
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR);

			light = shovelerLightPointCreate(shaderCache, *position, width, height, samples, ambientFactor, exponentialFactor, color);
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

static bool liveUpdateLightPositionDependency(ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, ShovelerComponent *dependencyComponent)
{
	ShovelerLight *light = (ShovelerLight *) component->data;

	ShovelerComponent *positionComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION);
	assert(positionComponent != NULL);
	const ShovelerVector3 *position = shovelerComponentGetPosition(positionComponent);

	shovelerLightUpdatePosition(light, *position);

	return false; // don't propagate
}
