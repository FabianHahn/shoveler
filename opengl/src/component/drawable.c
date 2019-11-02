#include "shoveler/component/drawable.h"

#include <string.h> // strcmp

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
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdDrawable, activateDrawableComponent, deactivateDrawableComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentDrawableOptionKeyType, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentDrawableOptionKeyTilesWidth, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentDrawableOptionKeyTilesHeight, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerDrawable *shovelerComponentGetDrawable(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdDrawable) == 0);

	return component->data;
}

static void *activateDrawableComponent(ShovelerComponent *component)
{
	ShovelerComponentDrawableType type = shovelerComponentGetConfigurationValueInt(component, shovelerComponentDrawableOptionKeyType);
	switch(type) {
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE:
			return shovelerDrawableCubeCreate();
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD:
			return shovelerDrawableQuadCreate();
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT:
			return shovelerDrawablePointCreate();
		case SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES: {
			int tilesWidth = shovelerComponentGetConfigurationValueInt(component, shovelerComponentDrawableOptionKeyTilesWidth);
			int tilesHeight = shovelerComponentGetConfigurationValueInt(component, shovelerComponentDrawableOptionKeyTilesHeight);
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
