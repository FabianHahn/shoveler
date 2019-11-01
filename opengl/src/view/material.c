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
	if(!shovelerViewHasComponentType(entity->view, shovelerViewMaterialComponentTypeName)) {
		shovelerViewAddComponentType(entity->view, shovelerComponentCreateMaterialType());
	}

	ShovelerComponent *component = shovelerViewEntityAddComponent(entity, shovelerViewMaterialComponentTypeName);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, ShovelerComponentMaterialTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewMaterialColorOptionKey, configuration->color);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureOptionKey, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureSamplerOptionKey, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTilemapOptionKey, configuration->tilemapEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialCanvasOptionKey, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey, configuration->canvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
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

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, ShovelerComponentMaterialTypeOptionKey);
	switch(outputConfiguration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, shovelerViewMaterialColorOptionKey);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			outputConfiguration->textureEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTextureOptionKey);
			outputConfiguration->textureSamplerEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTextureSamplerOptionKey);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			outputConfiguration->tilemapEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialTilemapOptionKey);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, shovelerViewMaterialCanvasOptionKey);
			outputConfiguration->canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey);
			outputConfiguration->canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
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
		shovelerLogWarning("Trying to update material of entity %lld which does not have a material, ignoring.", entity->id);
		return false;
	}

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, ShovelerComponentMaterialTypeOptionKey, configuration->type);

	switch(configuration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, shovelerViewMaterialColorOptionKey, configuration->color);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureOptionKey, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTextureSamplerOptionKey, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialTilemapOptionKey, configuration->tilemapEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, shovelerViewMaterialCanvasOptionKey, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionPositionOptionKey, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, shovelerViewMaterialCanvasRegionSizeOptionKey, configuration->canvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
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
		shovelerLogWarning("Trying to remove material from entity %lld which does not have a material, ignoring.", entity->id);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewMaterialComponentTypeName);
}
