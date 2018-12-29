#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/texture.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/view/canvas.h"
#include "shoveler/view/chunk.h"
#include "shoveler/view/material.h"
#include "shoveler/view/texture.h"
#include "shoveler/view/tilemap.h"
#include "shoveler/texture.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewMaterialConfiguration configuration;
	ShovelerMaterial *material;
} ComponentData;

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);
static const char *materialTypeToDataComponentName(ShovelerViewMaterialType type);

bool shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add material to entity %lld which already has a material, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = malloc(sizeof(ComponentData));
	componentData->configuration = configuration;
	componentData->material = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewMaterialComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	const char *dataComponentName = materialTypeToDataComponentName(componentData->configuration.type);
	if(dataComponentName != NULL) {
		shovelerViewComponentAddDependency(component, configuration.dataEntityId, dataComponentName);
	}

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component == NULL) {
		return NULL;
	}

	ComponentData *componentData = component->data;
	return componentData->material;
}

const ShovelerViewMaterialConfiguration *shovelerViewEntityGetMaterialConfiguration(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component == NULL) {
		return NULL;
	}

	ComponentData *componentData = component->data;
	return &componentData->configuration;
}

bool shovelerViewEntityUpdateMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update material of entity %lld which does not have a material, ignoring.", entity->entityId);
		return false;
	}

	ComponentData *componentData = component->data;

	shovelerViewComponentDeactivate(component);

	const char *dataComponentName = materialTypeToDataComponentName(componentData->configuration.type);
	if(dataComponentName != NULL) {
		if(!shovelerViewComponentRemoveDependency(component, configuration.dataEntityId, dataComponentName)) {
			return false;
		}
	}

	componentData->configuration = configuration;

	dataComponentName = materialTypeToDataComponentName(componentData->configuration.type);
	if(dataComponentName != NULL) {
		shovelerViewComponentAddDependency(component, configuration.dataEntityId, dataComponentName);
	}

	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveMaterial(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove material from entity %lld which does not have a material, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewMaterialComponentName);
}

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	switch (componentData->configuration.type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR:
			componentData->material = shovelerMaterialColorCreate(componentData->configuration.color);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE: {
			ShovelerViewEntity *textureEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.dataEntityId);
			assert(textureEntity != NULL);
			ShovelerTexture *texture = shovelerViewEntityGetTexture(textureEntity);
			assert(texture != NULL);
			ShovelerSampler *sampler = shovelerViewEntityGetTextureSampler(textureEntity);
			assert(sampler != NULL);

			componentData->material = shovelerMaterialTextureCreate(texture, false, sampler, false);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE:
			componentData->material = shovelerMaterialParticleCreate(componentData->configuration.color);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP: {
			ShovelerViewEntity *tilemapEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.dataEntityId);
			assert(tilemapEntity != NULL);
			ShovelerTilemap *tilemap = shovelerViewEntityGetTilemap(tilemapEntity);
			assert(tilemap != NULL);

			componentData->material = shovelerMaterialTilemapCreate();
			shovelerMaterialTilemapSetActive(componentData->material, tilemap);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CANVAS: {
			ShovelerViewEntity *canvasEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.dataEntityId);
			assert(canvasEntity != NULL);
			ShovelerCanvas *canvas = shovelerViewEntityGetCanvas(canvasEntity);
			assert(canvas != NULL);

			componentData->material = shovelerMaterialCanvasCreate();
			shovelerMaterialCanvasSetActive(componentData->material, canvas);
			shovelerMaterialCanvasSetActiveRegion(componentData->material, componentData->configuration.canvasRegionPosition, componentData->configuration.canvasRegionSize);
		} break;
		case SHOVELER_VIEW_MATERIAL_TYPE_CHUNK: {
			ShovelerViewEntity *chunkEntity = shovelerViewGetEntity(component->entity->view, componentData->configuration.dataEntityId);
			assert(chunkEntity != NULL);
			ShovelerChunk *chunk = shovelerViewEntityGetChunk(chunkEntity);
			assert(chunk != NULL);

			componentData->material = shovelerMaterialChunkCreate();
			shovelerMaterialChunkSetActive(componentData->material, chunk);
		} break;
		default:
			shovelerLogWarning("Trying to activate material with unknown material type %d, ignoring.", componentData->configuration.type);
			return false;
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	shovelerMaterialFree(componentData->material);
	componentData->material = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	ComponentData *componentData = componentDataPointer;

	if(componentData->material != NULL) {
		shovelerMaterialFree(componentData->material);
	}

	free(componentData);
}

static const char *materialTypeToDataComponentName(ShovelerViewMaterialType type)
{
	switch(type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE:
			return shovelerViewTextureComponentName;
		case SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP:
			return shovelerViewTilemapComponentName;
		case SHOVELER_VIEW_MATERIAL_TYPE_CANVAS:
			return shovelerViewCanvasComponentName;
		case SHOVELER_VIEW_MATERIAL_TYPE_CHUNK:
			return shovelerViewChunkComponentName;
		default:
			return NULL;
	}
}
