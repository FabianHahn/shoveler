#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/texture.h"
#include "shoveler/view/drawable.h"
#include "shoveler/view/material.h"
#include "shoveler/view/model.h"
#include "shoveler/view/position.h"
#include "shoveler/view/scene.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/view.h"

typedef struct {
	ShovelerViewModelConfiguration configuration;
	ShovelerModel *model;
	ShovelerViewComponentCallback *positionCallback;
} ModelComponentData;

static void updatePositionIfAvailable(ShovelerViewEntity *entity, ModelComponentData *modelComponentData);
static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *modelComponentDataPointer);
static bool activateComponent(ShovelerViewComponent *component, void *modelComponentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *modelComponentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *modelComponentDataPointer);

bool shovelerViewEntityAddModel(ShovelerViewEntity *entity, ShovelerViewModelConfiguration configuration)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add model to entity %lld which already has a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = malloc(sizeof(ModelComponentData));
	modelComponentData->configuration = configuration;
	modelComponentData->model = NULL;
	modelComponentData->positionCallback = NULL;

	component = shovelerViewEntityAddComponent(entity, shovelerViewModelComponentName, modelComponentData, activateComponent, deactivateComponent, freeComponent);
	assert(component != NULL);

	shovelerViewComponentAddDependency(component, entity->entityId, shovelerViewPositionComponentName);
	shovelerViewComponentAddDependency(component, configuration.drawableEntityId, shovelerViewDrawableComponentName);
	shovelerViewComponentAddDependency(component, configuration.materialEntityId, shovelerViewMaterialComponentName);

	shovelerViewComponentActivate(component);
	return true;
}

ShovelerModel *shovelerViewEntityGetModel(ShovelerViewEntity *entity)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		return NULL;
	}

	ModelComponentData *modelComponentData = component->data;
	return modelComponentData->model;
}

bool shovelerViewEntityUpdateModelDrawableEntityId(ShovelerViewEntity *entity, long long int drawableEntityId)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model drawable entity id of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;

	shovelerViewComponentDeactivate(component);
	if(!shovelerViewComponentRemoveDependency(component, modelComponentData->configuration.drawableEntityId, shovelerViewDrawableComponentName)) {
		return false;
	}

	modelComponentData->configuration.drawableEntityId = drawableEntityId;
	shovelerViewComponentAddDependency(component, drawableEntityId, shovelerViewDrawableComponentName);
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelMaterialEntityId(ShovelerViewEntity *entity, long long int materialEntityId)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model material entity id of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;

	shovelerViewComponentDeactivate(component);
	if(!shovelerViewComponentRemoveDependency(component, modelComponentData->configuration.materialEntityId, shovelerViewMaterialComponentName)) {
		return false;
	}

	modelComponentData->configuration.materialEntityId = materialEntityId;
	shovelerViewComponentAddDependency(component, materialEntityId, shovelerViewMaterialComponentName);
	shovelerViewComponentActivate(component);

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelRotation(ShovelerViewEntity *entity, ShovelerVector3 rotation)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model rotation of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.rotation = rotation;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->rotation = rotation;
		shovelerModelUpdateTransformation(modelComponentData->model);
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelScale(ShovelerViewEntity *entity, ShovelerVector3 scale)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model scale of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.scale = scale;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->scale = scale;
		shovelerModelUpdateTransformation(modelComponentData->model);
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelVisible(ShovelerViewEntity *entity, bool visible)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model visibility of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.visible = visible;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->visible = visible;
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelEmitter(ShovelerViewEntity *entity, bool emitter)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model emitter of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.emitter = emitter;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->emitter = emitter;
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelScreenspace(ShovelerViewEntity *entity, bool screenspace)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model screenspace of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.screenspace = screenspace;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->screenspace = screenspace;
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityUpdateModelPolygonMode(ShovelerViewEntity *entity, GLuint polygonMode)
{
	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model polygon mode of entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.polygonMode = polygonMode;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->polygonMode = polygonMode;
	}

	shovelerViewComponentUpdate(component);
	return true;
}

bool shovelerViewEntityRemoveModel(ShovelerViewEntity *entity)
{
	assert(shovelerViewHasScene(entity->view));

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove model from entity %lld which does not have a model, ignoring.", entity->entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentName);
}

static void updatePositionIfAvailable(ShovelerViewEntity *entity, ModelComponentData *modelComponentData)
{
	ShovelerViewComponent *positionComponent = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(positionComponent != NULL) {
		positionCallback(positionComponent, SHOVELER_VIEW_COMPONENT_CALLBACK_USER, modelComponentData);
	}
}

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *modelComponentDataPointer)
{
	ModelComponentData *modelComponentData = modelComponentDataPointer;
	ShovelerModel *model = modelComponentData->model;

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(positionComponent->entity);

	model->translation.values[0] = position->x;
	model->translation.values[1] = position->y;
	model->translation.values[2] = position->z;
	shovelerModelUpdateTransformation(model);
}

static bool activateComponent(ShovelerViewComponent *component, void *modelComponentDataPointer)
{
	ModelComponentData *modelComponentData = modelComponentDataPointer;
	assert(shovelerViewHasScene(component->entity->view));

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(component->entity);

	ShovelerViewEntity *drawableEntity = shovelerViewGetEntity(component->entity->view, modelComponentData->configuration.drawableEntityId);
	assert(drawableEntity != NULL);
	ShovelerDrawable *drawable = shovelerViewEntityGetDrawable(drawableEntity);
	assert(drawable != NULL);

	ShovelerViewEntity *materialEntity = shovelerViewGetEntity(component->entity->view, modelComponentData->configuration.materialEntityId);
	assert(materialEntity != NULL);
	ShovelerMaterial *material = shovelerViewEntityGetMaterial(materialEntity);
	assert(material != NULL);

	modelComponentData->model = shovelerModelCreate(drawable, material);
	modelComponentData->model->translation.values[0] = position->x;
	modelComponentData->model->translation.values[1] = position->y;
	modelComponentData->model->translation.values[2] = position->z;
	modelComponentData->model->rotation = modelComponentData->configuration.rotation;
	modelComponentData->model->scale = modelComponentData->configuration.scale;
	modelComponentData->model->visible = modelComponentData->configuration.visible;
	modelComponentData->model->emitter = modelComponentData->configuration.emitter;
	modelComponentData->model->screenspace = modelComponentData->configuration.screenspace;
	modelComponentData->model->castsShadow = modelComponentData->configuration.castsShadow;
	modelComponentData->model->polygonMode = modelComponentData->configuration.polygonMode;
	shovelerModelUpdateTransformation(modelComponentData->model);

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneAddModel(scene, modelComponentData->model);

	modelComponentData->positionCallback = shovelerViewEntityAddCallback(component->entity, shovelerViewPositionComponentName, &positionCallback, modelComponentData);
	return true;
}

static void deactivateComponent(ShovelerViewComponent *component, void *modelComponentDataPointer)
{
	ModelComponentData *modelComponentData = modelComponentDataPointer;
	assert(shovelerViewHasScene(component->entity->view));

	ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
	shovelerSceneRemoveModel(scene, modelComponentData->model);
	modelComponentData->model = NULL;

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, modelComponentData->positionCallback);
	modelComponentData->positionCallback = NULL;
}

static void freeComponent(ShovelerViewComponent *component, void *modelComponentDataPointer)
{
	assert(shovelerViewHasScene(component->entity->view));

	ModelComponentData *modelComponentData = modelComponentDataPointer;

	ShovelerModel *model = modelComponentData->model;
	if(model != NULL) {
		ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
		shovelerSceneRemoveModel(scene, model);
	}

	if(modelComponentData->positionCallback != NULL) {
		shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, modelComponentData->positionCallback);
	}

	free(modelComponentData);
}
