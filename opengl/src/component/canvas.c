#include "shoveler/component/canvas.h"

#include <string.h> // strcmp

#include "shoveler/component/tile_sprite.h"
#include "shoveler/canvas.h"
#include "shoveler/component.h"

static void *activateCanvasComponent(ShovelerComponent *component);
static void deactivateCanvasComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateCanvasType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewCanvasComponentTypeName, activateCanvasComponent, deactivateCanvasComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewCanvasTileSpritesOptionKey, shovelerViewTileSpriteComponentTypeName, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return componentType;
}

ShovelerCanvas *shovelerComponentGetCanvas(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerViewCanvasComponentTypeName) == 0);

	return component->data;
}

static void *activateCanvasComponent(ShovelerComponent *component)
{
	ShovelerCanvas *canvas = shovelerCanvasCreate();

	const ShovelerComponentConfigurationValue *tileSpritesValue = shovelerComponentGetConfigurationValue(component, shovelerViewCanvasTileSpritesOptionKey);
	assert(tileSpritesValue != NULL);

	for(size_t i = 0; i < tileSpritesValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *tileSpriteComponent = shovelerComponentGetArrayDependency(component, shovelerViewCanvasTileSpritesOptionKey, i);
		assert(tileSpriteComponent != NULL);
		ShovelerCanvasTileSprite *tileSprite = shovelerComponentGetTileSprite(tileSpriteComponent);
		assert(tileSprite != NULL);

		shovelerCanvasAddTileSprite(canvas, tileSprite);
	}

	return canvas;
}

static void deactivateCanvasComponent(ShovelerComponent *component)
{
	shovelerCanvasFree(component->data);
}
