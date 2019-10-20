#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/texture.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk.h"
#include "shoveler/view/material.h"
#include "shoveler/view/sampler.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/view/texture.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"
#include "shoveler/view.h"

static void *activateMaterialComponent(ShovelerComponent *component);
static void deactivateMaterialComponent(ShovelerComponent *component);

ShovelerComponent *shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerViewMaterialComponentTypeName)) {
		ShovelerComponentType *componentType = shovelerComponentTypeCreate(shovelerViewMaterialComponentTypeName, activateMaterialComponent, deactivateMaterialComponent, /* requiresAuthority */ false);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewMaterialTypeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_INT, /* isOptional */ false, /* liveUpdate */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewMaterialTextureOptionKey, shovelerViewTextureComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewMaterialTextureSamplerOptionKey, shovelerViewSamplerComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewMaterialTilemapOptionKey, shovelerViewTilemapComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewMaterialCanvasOptionKey, shovelerViewCanvasComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddDependencyConfigurationOption(componentType, shovelerViewMaterialChunkOptionKey, shovelerViewChunkComponentTypeName, /* isArray */ false, /* isOptional */ true, /* liveUpdate */ NULL, /* updateDependency */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewMaterialColorOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR3, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewMaterialCanvasRegionPositionOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerComponentTypeAddConfigurationOption(componentType, shovelerViewMaterialCanvasRegionSizeOptionKey, SHOVELER_COMPONENT_CONFIGURATION_OPTION_TYPE_VECTOR2, /* isOptional */ true, /* liveUpdate */ NULL);
		shovelerViewAddComponentType(entity->view, componentType);
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewMaterialComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewMaterialTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR:
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewMaterialColorOptionKey, configuration->color);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureOptionKey, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureSamplerOptionKey, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTilemapOptionKey, configuration->tilemapEntityId);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialCanvasOptionKey, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey, configuration->canvasRegionSize);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CHUNK:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialChunkOptionKey, configuration->chunkEntityId);
			break;
		default:
			break;
	}

	shovelerComponentActivate(component);
	return component;
}

ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetMaterialConfiguration(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentTypeName);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerViewMaterialTypeOptionKey);
	switch(outputConfiguration->type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR:
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE:
			outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewMaterialColorOptionKey);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE:
			outputConfiguration->textureEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTextureOptionKey);
			outputConfiguration->textureSamplerEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTextureSamplerOptionKey);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP:
			outputConfiguration->tilemapEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTilemapOptionKey);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CANVAS:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialCanvasOptionKey);
			outputConfiguration->canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey);
			outputConfiguration->canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CHUNK:
			outputConfiguration->chunkEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialChunkOptionKey);
			break;
		default:
			break;
	}
	return true;
}

bool shovelerViewEntityUpdateMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update material of entity %lld which does not have a material, ignoring.", entity->entityId);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerViewMaterialTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR:
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewMaterialColorOptionKey, configuration->color);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureOptionKey, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureSamplerOptionKey, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTilemapOptionKey, configuration->tilemapEntityId);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialCanvasOptionKey, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey, configuration->canvasRegionSize);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CHUNK:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialChunkOptionKey, configuration->chunkEntityId);
			break;
		default:
			break;
	}
	return true;
}

bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentTypeName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove material from entity %lld which does not have a material, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewMaterialComponentTypeName);
}

static void *activateMaterialComponent(ShovelerComponent *component)
{
	assert(shovelerViewHasShaderCache(component->entity->view));

	ShovelerShaderCache *shaderCache = shovelerViewGetShaderCache(component->entity->view);

	ShovelerViewMaterialType type = shovelerComponentGetConfigurationValueInt(component, shovelerViewMaterialTypeOptionKey);
	ShovelerMaterial *material;
	switch (type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR: {
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewMaterialColorOptionKey);
			material = shovelerMaterialColorCreate(shaderCache, /* screenspace */ false, color);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE: {
			long long int textureEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTextureOptionKey);
			ShovelerViewEntity *textureEntity = shovelerViewGetEntity(component->entity->view, textureEntityId);
			assert(textureEntity != NULL);
			ShovelerTexture *texture = shovelerViewEntityGetTexture(textureEntity);
			assert(texture != NULL);

			long long int textureSamplerEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTextureSamplerOptionKey);
			ShovelerViewEntity *textureSamplerEntity = shovelerViewGetEntity(component->entity->view, textureSamplerEntityId);
			assert(textureSamplerEntity != NULL);
			ShovelerSampler *sampler = shovelerViewEntityGetSampler(textureSamplerEntity);
			assert(sampler != NULL);

			material = shovelerMaterialTextureCreate(shaderCache, /* screenspace */ false, texture, false, sampler, false);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE: {
			ShovelerVector3 color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewMaterialColorOptionKey);
			material = shovelerMaterialParticleCreate(shaderCache, color);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP: {
			long long int tilemapEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTilemapOptionKey);
			ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(component->entity->view, tilemapEntityId);
			assert(tilemapEntity != NULL);
			ShovelerTilemap *tilemap = shovelerViewEntityGetTilemap(tilemapEntity);
			assert(tilemap != NULL);

			material = shovelerMaterialTilemapCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialTilemapSetActive(material, tilemap);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CANVAS: {
			long long int canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialCanvasOptionKey);
			ShovelerVector2 canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey);
			ShovelerVector2 canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey);
			ShovelerViewEntity *canvasEntity = shovelerViewGetEntity(component->entity->view, canvasEntityId);
			assert(canvasEntity != NULL);
			ShovelerCanvas *canvas = shovelerViewEntityGetCanvas(canvasEntity);
			assert(canvas != NULL);

			material = shovelerMaterialCanvasCreate(shaderCache, /* screenspace */ false);
			shovelerMaterialCanvasSetActive(material, canvas);
			shovelerMaterialCanvasSetActiveRegion(material, canvasRegionPosition, canvasRegionSize);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CHUNK: {
			long long int chunkEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialChunkOptionKey);
			ShovelerViewEntity *chunkEntity = shovelerViewGetEntity(component->entity->view, chunkEntityId);
			assert(chunkEntity != NULL);
			ShovelerChunk *chunk = shovelerViewEntityGetChunk(chunkEntity);
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
