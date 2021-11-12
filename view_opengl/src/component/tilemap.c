#include "shoveler/component/tilemap.h"

#include "shoveler/component/tilemap_colliders.h"
#include "shoveler/component/tilemap_tiles.h"
#include "shoveler/component/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"
#include "shoveler/tilemap.h"

const char *const shovelerComponentTypeIdTilemap = "tilemap";

static void *activateTilemapComponent(ShovelerComponent *component);
static void deactivateTilemapComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilemapType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[3];
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES] = shovelerComponentTypeConfigurationOptionDependency("tiles", shovelerComponentTypeIdTilemapTiles, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS] = shovelerComponentTypeConfigurationOptionDependency("colliders", shovelerComponentTypeIdTilemapColliders, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS] = shovelerComponentTypeConfigurationOptionDependency("tilesets", shovelerComponentTypeIdTileset, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdTilemap, activateTilemapComponent, /* update */ NULL, deactivateTilemapComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerTilemap *shovelerComponentGetTilemap(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdTilemap);

	return component->data;
}

static void *activateTilemapComponent(ShovelerComponent *component)
{
	ShovelerComponent *tilesComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILES);
	assert(tilesComponent != NULL);
	ShovelerTexture *tiles = shovelerComponentGetTilemapTiles(tilesComponent);
	assert(tiles != NULL);
	int numTilesColumns = tiles->width;
	int numTilesRows = tiles->height;

	ShovelerComponent *collidersComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_COLLIDERS);
	assert(collidersComponent != NULL);
	const bool *colliders = shovelerComponentGetTilemapColliders(collidersComponent);
	assert(colliders != NULL);
	int numCollidersColumns = shovelerComponentGetConfigurationValueInt(collidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_COLUMNS);
	int numCollidersRows = shovelerComponentGetConfigurationValueInt(collidersComponent, SHOVELER_COMPONENT_TILEMAP_COLLIDERS_OPTION_NUM_ROWS);

	if(numTilesColumns != numCollidersColumns || numTilesRows != numCollidersRows) {
		shovelerLogWarning("Failed to activate tilemap %lld because dependency tiles dimensions don't match dependency colliders dimensions.", component->entityId);
		return NULL;
	}

	ShovelerTilemap *tilemap = shovelerTilemapCreate(tiles, colliders);

	const ShovelerComponentConfigurationValue *tilesetsValue = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS);
	assert(tilesetsValue != NULL);

	for(int i = 0; i < tilesetsValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *tilesetComponent = shovelerComponentGetArrayDependency(component, SHOVELER_COMPONENT_TILEMAP_OPTION_ID_TILESETS, i);
		ShovelerTileset *tileset = shovelerComponentGetTileset(tilesetComponent);
		assert(tileset != NULL);

		shovelerTilemapAddTileset(tilemap, tileset);
	}

	return tilemap;
}

static void deactivateTilemapComponent(ShovelerComponent *component)
{
	ShovelerTilemap *tilemap = (ShovelerTilemap *) component->data;

	shovelerTilemapFree(tilemap);
}
