#include "shoveler/component/drawable.h"

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/drawable/tiles.h"
#include "shoveler/component.h"
#include "shoveler/drawable.h"
#include "shoveler/log.h"

const char *const shovelerComponentTypeIdDrawable = "drawable";

static void *activateDrawableComponent(ShovelerComponent *component);
static void deactivateDrawableComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateDrawableType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TILES_WIDTH] = shovelerComponentTypeConfigurationOption("tiles_width", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TILES_HEIGHT] = shovelerComponentTypeConfigurationOption("tiles_height", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdDrawable, activateDrawableComponent, deactivateDrawableComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerDrawable *shovelerComponentGetDrawable(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdDrawable);

	return component->data;
}

static void *activateDrawableComponent(ShovelerComponent *component)
{
	ShovelerComponentDrawableType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE);
	switch(type) {
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE:
			return shovelerDrawableCubeCreate();
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD:
			return shovelerDrawableQuadCreate();
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT:
			return shovelerDrawablePointCreate();
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES: {
			int tilesWidth = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TILES_WIDTH);
			int tilesHeight = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TILES_HEIGHT);
			return shovelerDrawableTilesCreate(tilesWidth, tilesHeight);
		}
		default:
			shovelerLogWarning("Failed to activate drawable with unknown type %d.", type);
			return NULL;
	}
}

static void deactivateDrawableComponent(ShovelerComponent *component)
{
	shovelerDrawableFree(component->data);
}
