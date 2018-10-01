#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/view/material.h"
#include "shoveler/log.h"

typedef struct {
	ShovelerViewMaterialConfiguration configuration;
	ShovelerMaterial *material;
} MaterialComponentData;

static bool activateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer);

bool shovelerViewEntityAddMaterial(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add drawable to entity %lld which already has a material, ignoring.", entity->entityId);
		return false;
	}

	MaterialComponentData *componentData = malloc(sizeof(MaterialComponentData));
	componentData->configuration = configuration;
	componentData->material = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewMaterialComponentName, componentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerMaterial *shovelerViewEntityGetMaterial(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component == NULL) {
		return NULL;
	}

	MaterialComponentData *componentData = component->data;
	return componentData->material;
}

bool shovelerViewEntityUpdateMaterialConfiguration(ShovelerViewEntity *entity, ShovelerViewMaterialConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewMaterialComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update material of entity %lld which does not have a material, ignoring.", entity->entityId);
		return false;
	}

	shovelerViewComponentDeactivate(component);

	MaterialComponentData *componentData = component->data;
	componentData->configuration = configuration;
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
	MaterialComponentData *componentData = componentDataPointer;

	switch (componentData->configuration.type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR:
			componentData->material = shovelerMaterialColorCreate(componentData->configuration.color);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE:
			shovelerLogWarning("Trying to activate material with unsupported texture type, ignoring.");
			return false;
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE:
			componentData->material = shovelerMaterialParticleCreate(componentData->configuration.color);
			break;
		default:
			shovelerLogWarning("Trying to activate material with unknown material type %d, ignoring.", componentData->configuration.type);
			return false;
	}

	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	MaterialComponentData *componentData = componentDataPointer;

	shovelerMaterialFree(componentData->material);
	componentData->material = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *componentDataPointer)
{
	MaterialComponentData *componentData = componentDataPointer;

	if(componentData->material != NULL) {
		shovelerMaterialFree(componentData->material);
	}

	free(componentData);
}
