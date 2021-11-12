#include "shoveler/component/material.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/sampler.h"
#include "shoveler/component/shader_cache.h"
#include "shoveler/component/texture.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/material/canvas.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/text.h"
#include "shoveler/material/texture.h"
#include "shoveler/material/texture_sprite.h"
#include "shoveler/material/tile_sprite.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/shader_cache.h"

const char *const shovelerComponentTypeIdMaterial = "material";

static void *activateMaterialComponent(ShovelerComponent *component);
static void deactivateMaterialComponent(ShovelerComponent *component);
static bool validateOptionalConfigurationOption(ShovelerComponent *component, ShovelerComponentMaterialOptionId configurationOption, const char *typeDescription);

ShovelerComponentType *shovelerComponentCreateMaterialType()
{
	ShovelerComponentTypeConfigurationOption configurationOptions[10];
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE] = shovelerComponentTypeConfigurationOption("type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_TYPE] = shovelerComponentTypeConfigurationOption("texture_type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SPRITE_TYPE] = shovelerComponentTypeConfigurationOption("texture_sprite_type", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE] = shovelerComponentTypeConfigurationOptionDependency("texture", shovelerComponentTypeIdTexture, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER] = shovelerComponentTypeConfigurationOptionDependency("texture_sampler", shovelerComponentTypeIdSampler, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP] = shovelerComponentTypeConfigurationOptionDependency("tilemap", shovelerComponentTypeIdTilemap, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS] = shovelerComponentTypeConfigurationOptionDependency("canvas", shovelerComponentTypeIdCanvas, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* liveUpdateDependency */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR] = shovelerComponentTypeConfigurationOption("color", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR4, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION] = shovelerComponentTypeConfigurationOption("canvas_region_position", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);
	configurationOptions[SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE] = shovelerComponentTypeConfigurationOption("canvas_region_size", SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);

	return shovelerComponentTypeCreate(shovelerComponentTypeIdMaterial, activateMaterialComponent, /* update */ NULL, deactivateMaterialComponent, /* requiresAuthority */ false, sizeof(configurationOptions) / sizeof(configurationOptions[0]), configurationOptions);
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
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, "color")) {
				return NULL;
			}

			ShovelerVector4 color = shovelerComponentGetConfigurationValueVector4(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
			material = shovelerMaterialColorCreate(shaderCache, /* screenspace */ false, color);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE: {
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE, "texture")) {
				return NULL;
			}
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER, "texture")) {
				return NULL;
			}
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_TYPE, "texture")) {
				return NULL;
			}

			ShovelerMaterialTextureType textureType = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_TYPE);
			if(textureType == SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK) {
				if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, "texture with texture type alpha mask")) {
					return NULL;
				}
			}

			ShovelerComponent *textureComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE);
			assert(textureComponent != NULL);
			ShovelerTexture *texture = shovelerComponentGetTexture(textureComponent);
			assert(texture != NULL);

			ShovelerComponent *textureSamplerComponent = shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER);
			assert(textureSamplerComponent != NULL);
			ShovelerSampler *sampler = shovelerComponentGetSampler(textureSamplerComponent);
			assert(sampler != NULL);

			material = shovelerMaterialTextureCreate(shaderCache, /* screenspace */ false, textureType, texture, false, sampler, false);

			if(textureType == SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK) {
				ShovelerVector4 color = shovelerComponentGetConfigurationValueVector4(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
				shovelerMaterialTextureSetAlphaMaskColor(material, color);
			}
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE: {
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, "particle")) {
				return NULL;
			}

			ShovelerVector4 color = shovelerComponentGetConfigurationValueVector4(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
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
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION, "canvas")) {
				return NULL;
			}
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE, "canvas")) {
				return NULL;
			}
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS, "canvas")) {
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
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE_SPRITE: {
			if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SPRITE_TYPE, "texture sprite")) {
				return NULL;
			}

			ShovelerMaterialTextureSpriteType textureSpriteType = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SPRITE_TYPE);
			if(textureSpriteType == SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK) {
				if(!validateOptionalConfigurationOption(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, "texture sprite with texture type alpha mask")) {
					return NULL;
				}
			}

			material = shovelerMaterialTextureSpriteCreate(shaderCache, /* screenspace */ false, textureSpriteType);

			if(textureSpriteType == SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK) {
				ShovelerVector4 color = shovelerComponentGetConfigurationValueVector4(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
				shovelerMaterialTextureSpriteSetColor(material, color);
			}
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

static bool validateOptionalConfigurationOption(ShovelerComponent *component, ShovelerComponentMaterialOptionId configurationOptionId, const char *typeDescription) {
	assert(configurationOptionId < component->type->numConfigurationOptions);

	const ShovelerComponentTypeConfigurationOption *configurationOption = &component->type->configurationOptions[configurationOptionId];
	assert(configurationOption->isOptional);

	if(!shovelerComponentHasConfigurationValue(component, configurationOptionId)) {
		shovelerLogWarning("Failed to activate material component of entity %lld: Type is set to %s, but no %s option is provided.", component->entityId, typeDescription, configurationOption->dependencyComponentTypeId);
		return false;
	}

	return true;
}
