#include "shoveler/component/material.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/chunk.h"
#include "shoveler/component/sampler.h"
#include "shoveler/component/shader_cache.h"
#include "shoveler/component/texture.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/material/canvas.h"
#include "shoveler/material/chunk.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/text.h"
#include "shoveler/material/texture.h"
#include "shoveler/material/tile_sprite.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/shader_cache.h"

const char *const shovelerComponentTypeIdMaterial = "material";

static void *activateMaterialComponent(ShovelerComponent *component);
static void deactivateMaterialComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateMaterialType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[9];
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE] = shovelerComponentTypeConfigurationOptionDependency("texture", shovelerComponentTypeIdTexture, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER] = shovelerComponentTypeConfigurationOptionDependency("texture_sampler", shovelerComponentTypeIdSampler, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP] = shovelerComponentTypeConfigurationOptionDependency("tilemap", shovelerComponentTypeIdTilemap, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS] = shovelerComponentTypeConfigurationOptionDependency("canvas", shovelerComponentTypeIdCanvas, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CHUNK] = shovelerComponentTypeConfigurationOptionDependency("chunk", shovelerComponentTypeIdChunk, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR] = shovelerComponentTypeConfigurationOption("color", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION] = shovelerComponentTypeConfigurationOption("canvas_region_position", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE] = shovelerComponentTypeConfigurationOption("canvas_region_size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdMaterial, activateMaterialComponent, deactivateMaterialComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
}

ShovelerMaterial *shovelerComponentGetMaterial(ShovelerComponent *component)
{
	assert(component->type->id == shovelerComponentTypeIdMaterial);

	return component->data;
}

static void *activateMaterialComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);

	ShovelerComponentMaterialType type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE);
	ShovelerMaterial *material;
	switch (type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR: {
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
			material = shovelerMaterialColorCreate(shaderCache, /* screenspace */ false, color);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE: {
			ShovelerComponent *textureComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE);
			assert(textureComponent != NULL);
			ShovelerTexture *texture = shovelerComponentGetTexture(textureComponent);
			assert(texture != NULL);

			ShovelerComponent *textureSamplerComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER);
			assert(textureSamplerComponent != NULL);
			ShovelerSampler *sampler = shovelerComponentGetSampler(textureSamplerComponent);
			assert(sampler != NULL);

			material = shovelerMaterialTextureCreate(shaderCache, /* screenspace */ false, texture, false, sampler, false);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE: {
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
			material = shovelerMaterialParticleCreate(shaderCache, color);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP: {
			material = shovelerMaterialTilemapCreate(shaderCache, /* screenspace */ false);

			bool hasTilemap = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP);
			if(hasTilemap) {
				ShovelerComponent *tilemapComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP);
				assert(tilemapComponent != NULL);
				ShovelerTilemap *tilemap = shovelerComponentGetTilemap(tilemapComponent);
				assert(tilemap != NULL);
				shovelerMaterialTilemapSetActive(material, tilemap);
			}
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS: {
			bool hasRegionPosition = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION);
			bool hasRegionSize = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE);
			bool hasCanvas = shovelerComponentHasConfigurationValue(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS);

			if(!hasRegionPosition || !hasRegionSize || !hasCanvas) {
				shovelerLogWarning("Failed to activate material component of entity %lld: Type is set to canvas, but not all of canvas, region position, and region size options are provided.", component->entityId);
				return NULL;
			}

			ShovelerVector2 canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION);
			ShovelerVector2 canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE);
			ShovelerComponent *canvasComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS);
			assert(canvasComponent != NULL);
			ShovelerCanvas *canvas = shovelerComponentGetCanvas(canvasComponent);
			assert(canvas != NULL);

			material = shovelerMaterialCanvasCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialCanvasSetActive(material, canvas);
			shovelerMaterialCanvasSetActiveRegion(material, canvasRegionPosition, canvasRegionSize);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK: {
			ShovelerComponent *chunkComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CHUNK);
			assert(chunkComponent != NULL);
			ShovelerChunk *chunk = shovelerComponentGetChunk(chunkComponent);
			assert(chunk != NULL);

			material = shovelerMaterialChunkCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialChunkSetActive(material, chunk);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILE_SPRITE: {
			material = shovelerMaterialTileSpriteCreate(shaderCache, /* screenspace */ false);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXT: {
			material = shovelerMaterialTextCreate(shaderCache, /* screenspace */ false);
		} break;
		default:
			shovelerLogWarning("Trying to activate material with unknown material type %d, ignoring.", type);
			return NULL;
	}

	return material;
}

static void deactivateMaterialComponent(ShovelerComponent *component)
{
	ShovelerMaterial *material = (ShovelerMaterial *) component->data;

	shovelerMaterialFree(material);
}
