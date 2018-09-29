#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/texture.h"
#include "shoveler/view/drawable.h"
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

static ShovelerMaterial *createMaterial(ShovelerView *view, ShovelerViewMaterialConfiguration configuration);
static void updatePositionIfAvailable(ShovelerViewEntity *entity, ModelComponentData *modelComponentData);
static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *modelComponentDataPointer);
static bool activateComponent(ShovelerViewComponent *component, void *modelComponentDataPointer);
static void deactivateComponent(ShovelerViewComponent *component, void *modelComponentDataPointer);
static void freeComponent(ShovelerViewComponent *component, void *modelComponentDataPointer);

bool shovelerViewAddEntityModel(ShovelerView *view, long long int entityId, ShovelerViewModelConfiguration modelConfiguration)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to add model to non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component != NULL) {
		shovelerLogWarning("Trying to add model to entity %lld which already has a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = malloc(sizeof(ModelComponentData));
	modelComponentData->configuration = modelConfiguration;
	modelComponentData->model = NULL;
	modelComponentData->positionCallback = NULL;

	if (!shovelerViewEntityAddComponent(entity, shovelerViewModelComponentName, modelComponentData, activateComponent, deactivateComponent, freeComponent)) {
		free(modelComponentData);
		return false;
	}

	if(!shovelerViewEntityAddComponentDependency(entity, shovelerViewModelComponentName, entity->entityId, shovelerViewPositionComponentName)) {
		shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentName);
		return false;
	}

	if(!shovelerViewEntityAddComponentDependency(entity, shovelerViewModelComponentName, modelConfiguration.drawableEntityId, shovelerViewDrawableComponentName)) {
		shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentName);
		return false;
	}

	shovelerViewEntityActivateComponent(entity, shovelerViewModelComponentName);
	return true;
}

ShovelerModel *shovelerViewGetEntityModel(ShovelerView *view, long long int entityId)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		return NULL;
	}

	return shovelerViewEntityGetModel(entity);
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

bool shovelerViewUpdateEntityModelDrawableEntityId(ShovelerView *view, long long int entityId, long long int drawableEntityId)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model material of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model material of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;

	shovelerViewEntityDeactivateComponent(entity, shovelerViewModelComponentName);
	shovelerViewEntityRemoveComponentDependency(entity, shovelerViewModelComponentName, modelComponentData->configuration.drawableEntityId, shovelerViewDrawableComponentName);

	modelComponentData->configuration.drawableEntityId = drawableEntityId;
	shovelerViewEntityAddComponentDependency(entity, shovelerViewModelComponentName, drawableEntityId, shovelerViewDrawableComponentName);
	shovelerViewEntityActivateComponent(entity, shovelerViewModelComponentName);

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelMaterial(ShovelerView *view, long long int entityId, ShovelerViewMaterialConfiguration materialConfiguration)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model material of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model material of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.material = materialConfiguration;

	shovelerViewEntityDeactivateComponent(entity, shovelerViewModelComponentName);
	shovelerViewEntityActivateComponent(entity, shovelerViewModelComponentName);

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelRotation(ShovelerView *view, long long int entityId, ShovelerVector3 rotation)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model rotation of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model rotation of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.rotation = rotation;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->rotation = rotation;
		shovelerModelUpdateTransformation(modelComponentData->model);
	}

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelScale(ShovelerView *view, long long int entityId, ShovelerVector3 scale)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model scale of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model scale of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.scale = scale;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->scale = scale;
		shovelerModelUpdateTransformation(modelComponentData->model);
	}

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelVisible(ShovelerView *view, long long int entityId, bool visible)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model visibility of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model visibility of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.visible = visible;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->visible = visible;
	}

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelEmitter(ShovelerView *view, long long int entityId, bool emitter)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model emitter of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model emitter of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.emitter = emitter;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->emitter = emitter;
	}

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelScreenspace(ShovelerView *view, long long int entityId, bool screenspace)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model screenspace of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model screenspace of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.screenspace = screenspace;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->screenspace = screenspace;
	}

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewUpdateEntityModelPolygonMode(ShovelerView *view, long long int entityId, GLuint polygonMode)
{
	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model polygon mode of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model polygon mode of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	ModelComponentData *modelComponentData = component->data;
	modelComponentData->configuration.polygonMode = polygonMode;

	if(modelComponentData->model != NULL) {
		modelComponentData->model->polygonMode = polygonMode;
	}

	return shovelerViewEntityUpdateComponent(entity, shovelerViewModelComponentName);
}

bool shovelerViewRemoveEntityModel(ShovelerView *view, long long int entityId)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to remove model from non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to remove model from entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}

	return shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentName);
}

static ShovelerMaterial *createMaterial(ShovelerView *view, ShovelerViewMaterialConfiguration configuration)
{
	switch (configuration.type) {
		case SHOVELER_VIEW_MATERIAL_TYPE_COLOR:
			return shovelerMaterialColorCreate(configuration.color);
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE:
			shovelerLogWarning("Trying to create model with unsupported texture type, ignoring.", configuration.type);
			return NULL;
			break;
		case SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE:
			return shovelerMaterialParticleCreate(configuration.color);
			break;
		default:
			shovelerLogWarning("Trying to create model with unknown material type %d, ignoring.", configuration.type);
			return NULL;
	}
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

	ShovelerMaterial *material = createMaterial(component->entity->view, modelComponentData->configuration.material);
	if(material == NULL) {
		shovelerLogWarning("Trying to activate model to entity %lld but failed to create material, ignoring.", component->entity->entityId);
		return false;
	}

	ShovelerViewPosition *position = shovelerViewEntityGetPosition(component->entity);

	ShovelerDrawable *drawable = shovelerViewGetEntityDrawable(component->entity->view, modelComponentData->configuration.drawableEntityId);
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
	return false;
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

	ModelComponentData *modelComponentData = component->data;

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
