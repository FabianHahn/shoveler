#include "shoveler/component/tilemap.h"

#include <string.h> // strcmp

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
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeIdTilemap, activateTilemapComponent, deactivateTilemapComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentTilemapOptionKeyTiles, shovelerComponentTypeIdTilemapTiles, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentTilemapOptionKeyColliders, shovelerComponentTypeIdTilemapColliders, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentTilemapOptionKeyTilesets, shovelerComponentTypeIdTileset, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}

ShovelerTilemap *shovelerComponentGetTilemap(ShovelerComponent *component)
{
	assert(strcmp(component->type->id, shovelerComponentTypeIdTilemap) == 0);

	return component->data;
}

static void *activateTilemapComponent(ShovelerComponent *component)
{
	ShovelerComponent *tilesComponent = shovelerComponentGetDependency(component, shovelerComponentTilemapOptionKeyTiles);
	assert(tilesComponent != NULL);
	ShovelerTexture *tiles = shovelerComponentGetTilemapTiles(tilesComponent);
	assert(tiles != NULL);
	int numTilesColumns = tiles->width;
	int numTilesRows = tiles->height;

	ShovelerComponent *collidersComponent = shovelerComponentGetDependency(component, shovelerComponentTilemapOptionKeyColliders);
	assert(collidersComponent != NULL);
	const bool *colliders = shovelerComponentGetTilemapColliders(collidersComponent);
	assert(colliders != NULL);
	int numCollidersColumns = shovelerComponentGetConfigurationValueInt(collidersComponent, shovelerComponentTilemapCollidersOptionKeyNumColumns);
	int numCollidersRows = shovelerComponentGetConfigurationValueInt(collidersComponent, shovelerComponentTilemapCollidersOptionKeyNumRows);

	if(numTilesColumns != numCollidersColumns || numTilesRows != numCollidersRows) {
		shovelerLogWarning("Failed to activate tilemap %lld because dependency tiles dimensions don't match dependency colliders dimensions.", component->entityId);
		return NULL;
	}

	ShovelerTilemap *tilemap = shovelerTilemapCreate(tiles, colliders);

	const ShovelerComponentConfigurationValue *tilesetsValue = shovelerComponentGetConfigurationValue(component, shovelerComponentTilemapOptionKeyTilesets);
	assert(tilesetsValue != NULL);

	for(int i = 0; i < tilesetsValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *tilesetComponent = shovelerComponentGetArrayDependency(component, shovelerComponentTilemapOptionKeyTilesets, i);
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
