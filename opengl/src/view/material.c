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
	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, configuration->type);

	switch(configuration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, configuration->color);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP, configuration->tilemapEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE, configuration->canvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CHUNK, configuration->chunkEntityId);
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

	outputConfiguration->type = shovelerComponentGetConfigurationValueInt(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE);
	switch(outputConfiguration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			outputConfiguration->color = shovelerComponentGetConfigurationValueVector3(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			outputConfiguration->textureEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE);
			outputConfiguration->textureSamplerEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			outputConfiguration->tilemapEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			outputConfiguration->canvasEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS);
			outputConfiguration->canvasRegionPosition = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION);
			outputConfiguration->canvasRegionSize = shovelerComponentGetConfigurationValueVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
			outputConfiguration->chunkEntityId = shovelerComponentGetConfigurationValueEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CHUNK);
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

	shovelerComponentUpdateCanonicalConfigurationOptionInt(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, configuration->type);

	switch(configuration->type) {
		case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR:
		case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE:
			shovelerComponentUpdateCanonicalConfigurationOptionVector3(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, configuration->color);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE, configuration->textureEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER, configuration->textureSamplerEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TILEMAP, configuration->tilemapEntityId);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS, configuration->canvasEntityId);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_POSITION, configuration->canvasRegionPosition);
			shovelerComponentUpdateCanonicalConfigurationOptionVector2(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CANVAS_REGION_SIZE, configuration->canvasRegionSize);
			break;
		case SHOVELER_COMPONENT_MATERIAL_TYPE_CHUNK:
			shovelerComponentUpdateCanonicalConfigurationOptionEntityId(component, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_CHUNK, configuration->chunkEntityId);
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
