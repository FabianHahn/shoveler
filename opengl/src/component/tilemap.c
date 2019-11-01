#include "shoveler/component/tilemap.h"

#include <string.h> // strcmp

#include "shoveler/component/tilemap_colliders.h"
#include "shoveler/component/tilemap_tiles.h"
#include "shoveler/component/tileset.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"
#include "shoveler/tilemap.h"

static void *activateTilemapComponent(ShovelerComponent *component);
static void deactivateTilemapComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateTilemapType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewTilemapComponentTypeName, activateTilemapComponent, deactivateTilemapComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesOptionKey, shovelerViewTilemapTilesComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapCollidersOptionKey, shovelerViewTilemapCollidersComponentTypeName, /* isArray */ false, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewTilemapTilesetsOptionKey, shovelerViewTilesetComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);
}

ShovelerTilemap *shovelerComponentGetTilemap(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewTilemapComponentTypeName) == 0);

	return component->data;
}

static void *activateTilemapComponent(ShovelerComponent *component)
{
	ShovelerComponent *tilesComponent = shovelerComponentGetDependency(component, shovelerViewTilemapTilesOptionKey);
	assert(tilesComponent != NULL);
	ShovelerTexture *tiles = shovelerComponentGetTilemapTiles(tilesComponent);
	assert(tiles != NULL);
	int numTilesColumns = tiles->width;
	int numTilesRows = tiles->height;

	ShovelerComponent *collidersComponent = shovelerComponentGetDependency(component, shovelerViewTilemapCollidersOptionKey);
	assert(collidersComponent != NULL);
	const bool *colliders = shovelerComponentGetTilemapColliders(collidersComponent);
	assert(colliders != NULL);
	int numCollidersColumns = shovelerComponentGetConfigurationValueInt(collidersComponent, shovelerViewTilemapCollidersNumColumnsOptionKey);
	int numCollidersRows = shovelerComponentGetConfigurationValueInt(collidersComponent, shovelerViewTilemapCollidersNumRowsOptionKey);

	if(numTilesColumns != numCollidersColumns || numTilesRows != numCollidersRows) {
		shovelerLogWarning("Failed to activate tilemap %lld because dependency tiles dimensions don't match dependency colliders dimensions.", component->entityId);
		return NULL;
	}

	ShovelerTilemap *tilemap = shovelerTilemapCreate(tiles, colliders);

	const ShovelerComponentConfigurationValue *tilesetsValue = shovelerComponentGetConfigurationValue(component, shovelerViewTilemapTilesetsOptionKey);
	assert(tilesetsValue != NULL);

	for(int i = 0; i < tilesetsValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *tilesetComponent = shovelerComponentGetArrayDependency(component, shovelerViewTilemapTilesetsOptionKey, i);
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
