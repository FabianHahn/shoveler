#include "shoveler/component/material.h"

#include <string.h> // strcmp

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
#include "shoveler/material/texture.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/shader_cache.h"

static void *activateMaterialComponent(ShovelerComponent *component);
static void deactivateMaterialComponent(ShovelerComponent *component);

ShovelerComponentType *shovelerComponentCreateMaterialType()
{
	ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerComponentTypeNameMaterial, activateMaterialComponent, deactivateMaterialComponent, /* requiresAuthority */ false);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentMaterialOptionKeyType, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentMaterialOptionKeyTexture, shovelerComponentTypeNameTexture, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentMaterialOptionKeyTextureSampler, shovelerComponentTypeNameSampler, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentMaterialOptionKeyTilemap, shovelerComponentTypeNameTilemap, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentMaterialOptionKeyCanvas, shovelerComponentTypeNameCanvas, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerComponentMaterialOptionKeyChunk, shovelerComponentTypeNameChunk, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentMaterialOptionKeyColor, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentMaterialOptionKeyCanvasRegionPosition, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);
	shovelerComponentTypeAddConfigurationOption(componentType, shovelerComponentMaterialOptionKeyCanvasRegionSize, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);

	return componentType;
}

ShovelerMaterial *shovelerComponentGetMaterial(ShovelerComponent *component)
{
	assert(strcmp(component->type->name, shovelerComponentTypeNameMaterial) == 0);

	return component->data;
}

static void *activateMaterialComponent(ShovelerComponent *component)
{
	assert(shovelerComponentHasViewShaderCache(component));

	ShovelerShaderCache *shaderCache = shovelerComponentGetViewShaderCache(component);

	ShovelerComponentMaterialType type = shovelerComponentGetConfigurationValueInt(component, shovelerComponentMaterialOptionKeyType);
	ShovelerMaterial *material;
	switch (type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR: {
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentMaterialOptionKeyColor);
			material = shovelerMaterialColorCreate(shaderCache, /* screenspace */ false, color);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE: {
			ShovelerComponent *textureComponent = shovelerComponentGetDependency(component, shovelerComponentMaterialOptionKeyTexture);
			assert(textureComponent != NULL);
			ShovelerTexture *texture = shovelerComponentGetTexture(textureComponent);
			assert(texture != NULL);

			ShovelerComponent *textureSamplerComponent = shovelerComponentGetDependency(component, shovelerComponentMaterialOptionKeyTextureSampler);
			assert(textureSamplerComponent != NULL);
			ShovelerSampler *sampler = shovelerComponentGetSampler(textureSamplerComponent);
			assert(sampler != NULL);

			material = shovelerMaterialTextureCreate(shaderCache, /* screenspace */ false, texture, false, sampler, false);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE: {
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentMaterialOptionKeyColor);
			material = shovelerMaterialParticleCreate(shaderCache, color);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP: {
			ShovelerComponent *tilemapComponent = shovelerComponentGetDependency(component, shovelerComponentMaterialOptionKeyTilemap);
			assert(tilemapComponent != NULL);
			ShovelerTilemap *tilemap = shovelerComponentGetTilemap(tilemapComponent);
			assert(tilemap != NULL);

			material = shovelerMaterialTilemapCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialTilemapSetActive(material, tilemap);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS: {
			ShovelerVector2 canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionPosition);
			ShovelerVector2 canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionSize);
			ShovelerComponent *canvasComponent = shovelerComponentGetDependency(component, shovelerComponentMaterialOptionKeyCanvas);
			assert(canvasComponent != NULL);
			ShovelerCanvas *canvas = shovelerComponentGetCanvas(canvasComponent);
			assert(canvas != NULL);

			material = shovelerMaterialCanvasCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialCanvasSetActive(material, canvas);
			shovelerMaterialCanvasSetActiveRegion(material, canvasRegionPosition, canvasRegionSize);
		} break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK: {
			ShovelerComponent *chunkComponent = shovelerComponentGetDependency(component, shovelerComponentMaterialOptionKeyChunk);
			assert(chunkComponent != NULL);
			ShovelerChunk *chunk = shovelerComponentGetChunk(chunkComponent);
			assert(chunk != NULL);

			material = shovelerMaterialChunkCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialChunkSetActive(material, chunk);
		} break;
		default:
			shovelerLogWarning("Trying to activate material with unknown material type %d, ignoring.", type);
			return false;
	}

	return material;
}

static void deactivateMaterialComponent(ShovelerComponent *component)
{
	ShovelerMaterial *material = (ShovelerMaterial *) component->data;

	shovelerMaterialFree(material);
}
