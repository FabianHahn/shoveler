#include "shoveler/component/canvas.h"

#include "shoveler/component/tile_sprite.h"
#include "shoveler/canvas.h"
#include "shoveler/component.h"

const char *const shovelerComponentTypeIdCanvas = "canvas";

static void *activateCanvasComponent(ShovelerComponent *component);
static void deactivateCanvasComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateCanvasType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[1];
	configurationOptions[SHOVELER_COMPONENT_CANVAS_OPTION_ID_TILE_SPRITE] = shovelerComponentTypeConfigurationOptionDependency("tile_sprite", shovelerComponentTypeIdTileSprite, /* isArray */ true, /* isOptional */ false, /* liveUpdate */ NULL, /* updateDependency */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdCanvas, activateCanvasComponent, deactivateCanvasComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerCanvas *shovelerComponentGetCanvas(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdCanvas);

	return component->data;
}

static void *activateCanvasComponent(ShovelerComponent *component)
{
	ShovelerCanvas *canvas = shovelerCanvasCreate(/* numLayers */ 1);

	const ShovelerComponentConfigurationValue *tileSpritesValue = shovelerComponentGetConfigurationValue(component, SHOVELER_COMPONENT_CANVAS_OPTION_ID_TILE_SPRITE);
	assert(tileSpritesValue != NULL);

	for(int i = 0; i < tileSpritesValue->entityIdArrayValue.size; i++) {
		ShovelerComponent *tileSpriteComponent = shovelerComponentGetArrayDependency(component, SHOVELER_COMPONENT_CANVAS_OPTION_ID_TILE_SPRITE, i);
		assert(tileSpriteComponent != NULL);
		ShovelerSprite *tileSprite = shovelerComponentGetTileSprite(tileSpriteComponent);
		assert(tileSprite != NULL);

		shovelerCanvasAddSprite(canvas, /* layerId */ 0, tileSprite);
	}

	return canvas;
}

static void deactivateCanvasComponent(ShovelerComponent *component)
{
	shovelerCanvasFree(component->data);
}
