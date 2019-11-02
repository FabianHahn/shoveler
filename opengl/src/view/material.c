#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/view/chunk.h"
#include "shoveler/view/material.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/component.h"
#include "shoveler/log.h"
#include "shoveler/texture.h"
#include "shoveler/view.h"

ShovelerComponent *shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration)
{
	if(!shovelerViewHasComponentType(entity->view, shovelerComponentTypeIdMaterial)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateMaterialType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentMaterialOptionKeyType, configuration->type);

	switch(configuration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentMaterialOptionKeyColor, configuration->color);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyTexture, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyTextureSampler, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyTilemap, configuration->tilemapEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyCanvas, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionPosition, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionSize, configuration->canvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyChunk, configuration->chunkEntityId);
			break;
		default:
			break;
	}

	shovelerComponentActivate(component);
	return component;
}

ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdMaterial);
	if(component == NULL) {
		return NULL;
	}

	return component->data;
}

bool shovelerViewEntityGetMaterialConfiguration(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration *outputConfiguration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdMaterial);
	if(component == NULL) {
		return NULL;
	}

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, shovelerComponentMaterialOptionKeyType);
	switch(outputConfiguration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, shovelerComponentMaterialOptionKeyColor);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			outputConfiguration->textureEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentMaterialOptionKeyTexture);
			outputConfiguration->textureSamplerEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentMaterialOptionKeyTextureSampler);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			outputConfiguration->tilemapEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentMaterialOptionKeyTilemap);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentMaterialOptionKeyCanvas);
			outputConfiguration->canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionPosition);
			outputConfiguration->canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
			outputConfiguration->chunkEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerComponentMaterialOptionKeyChunk);
			break;
		default:
			break;
	}
	return true;
}

bool shovelerViewEntityUpdateMaterial(ShovelerViewEntity *entity, const ShovelerViewMaterialConfiguration *configuration)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdMaterial);
	if(component == NULL) {
		shovelerLogWarning("Trying to update material of entity %lld which does not have a material, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, shovelerComponentMaterialOptionKeyType, configuration->type);

	switch(configuration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerComponentMaterialOptionKeyColor, configuration->color);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyTexture, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyTextureSampler, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyTilemap, configuration->tilemapEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyCanvas, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionPosition, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerComponentMaterialOptionKeyCanvasRegionSize, configuration->canvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerComponentMaterialOptionKeyChunk, configuration->chunkEntityId);
			break;
		default:
			break;
	}
	return true;
}

bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity)
{
	ShovelerComponent *component = shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdMaterial);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove material from entity %lld which does not have a material, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerComponentTypeIdMaterial);
}
