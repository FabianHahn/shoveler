#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/point.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/texture.h"
#include "shoveler/view/drawables.h"
#include "shoveler/view/model.h"
#include "shoveler/view/position.h"
#include "shoveler/view/scene.h"
#include "shoveler/log.h"
#include "shoveler/model.h"
#include "shoveler/view.h"

static ShovelerDrawable *getDrawable(ShovelerView *view, ShovelerViewDrawableConfiguration configuration);
static ShovelerMaterial *createMaterial(ShovelerView *view, ShovelerViewMaterialConfiguration configuration);
static void updatePositionIfAvailable(ShovelerViewEntity *entity, ShovelerViewModel *modelComponentData);
static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *modelComponentDataPointer);
static void freeComponent(ShovelerViewComponent *component);

bool shovelerViewAddEntityModel(ShovelerView *view, long long int entityId, ShovelerViewModelConfiguration modelConfiguration)
{
	assert(shovelerViewHasScene(view));

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

	ShovelerDrawable *drawable = getDrawable(view, modelConfiguration.drawable);
	if(drawable == NULL) {
		shovelerLogWarning("Trying to add model to entity %lld but failed to create drawable, ignoring.", entityId);
		return false;
	}

	ShovelerMaterial *material = createMaterial(view, modelConfiguration.material);
	if(material == NULL) {
		shovelerLogWarning("Trying to add model to entity %lld but failed to create material, ignoring.", entityId);
		return false;
	}

	ShovelerModel *model = shovelerModelCreate(drawable, material);
	model->rotation = modelConfiguration.rotation;
	model->scale = modelConfiguration.scale;
	model->visible = modelConfiguration.visible;
	model->emitter = modelConfiguration.emitter;
	model->screenspace = modelConfiguration.screenspace;
	model->castsShadow = modelConfiguration.castsShadow;
	model->polygonMode = modelConfiguration.polygonMode;

	ShovelerScene *scene = shovelerViewGetScene(view);
	shovelerSceneAddModel(scene, model);

	ShovelerViewModel *modelComponentData = malloc(sizeof(ShovelerViewModel));
	modelComponentData->model = model;
	modelComponentData->positionCallback = shovelerViewEntityAddCallback(entity, shovelerViewPositionComponentName, &positionCallback, modelComponentData);

	if (!shovelerViewEntityAddComponent(entity, shovelerViewModelComponentName, modelComponentData, NULL, NULL, &freeComponent)) {
		freeComponent(component);
		return false;
	}

	if(!shovelerViewEntityAddComponentDependency(entity, shovelerViewModelComponentName, entity->entityId, shovelerViewPositionComponentName)) {
		shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentName);
		return false;
	}

	shovelerViewEntityActivateComponent(entity, shovelerViewModelComponentName);
	return true;
}

bool shovelerViewUpdateEntityModelDrawable(ShovelerView *view, long long int entityId, ShovelerViewDrawableConfiguration drawableConfiguration)
{
	assert(shovelerViewHasScene(view));

	ShovelerViewEntity *entity = shovelerViewGetEntity(view, entityId);
	if(entity == NULL) {
		shovelerLogWarning("Trying to update model drawable of non existing entity %lld, ignoring.", entityId);
		return false;
	}

	ShovelerViewComponent *component = shovelerViewEntityGetComponent(entity, shovelerViewModelComponentName);
	if(component == NULL) {
		shovelerLogWarning("Trying to update model drawable of entity %lld which does not have a model, ignoring.", entityId);
		return false;
	}
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *oldModel = modelComponentData->model;
	ShovelerDrawable *drawable = getDrawable(view, drawableConfiguration);
	modelComponentData->model = shovelerModelCreate(drawable, oldModel->material);
	modelComponentData->model->rotation = oldModel->rotation;
	modelComponentData->model->scale = oldModel->scale;
	modelComponentData->model->visible = oldModel->visible;
	modelComponentData->model->emitter = oldModel->emitter;
	modelComponentData->model->screenspace = oldModel->screenspace;
	modelComponentData->model->castsShadow = oldModel->castsShadow;
	modelComponentData->model->polygonMode = oldModel->polygonMode;

	ShovelerScene *scene = shovelerViewGetScene(view);
	shovelerSceneRemoveModel(scene, oldModel);
	shovelerSceneAddModel(scene, modelComponentData->model);

	updatePositionIfAvailable(entity, modelComponentData);
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *oldModel = modelComponentData->model;
	ShovelerMaterial *oldMaterial = oldModel->material;
	ShovelerMaterial *material = createMaterial(view, materialConfiguration);
	modelComponentData->model = shovelerModelCreate(oldModel->drawable, material);
	modelComponentData->model->rotation = oldModel->rotation;
	modelComponentData->model->scale = oldModel->scale;
	modelComponentData->model->visible = oldModel->visible;
	modelComponentData->model->emitter = oldModel->emitter;
	modelComponentData->model->screenspace = oldModel->screenspace;
	modelComponentData->model->castsShadow = oldModel->castsShadow;
	modelComponentData->model->polygonMode = oldModel->polygonMode;

	ShovelerScene *scene = shovelerViewGetScene(view);
	shovelerSceneRemoveModel(scene, oldModel);
	shovelerMaterialFree(oldMaterial);
	shovelerSceneAddModel(scene, modelComponentData->model);

	updatePositionIfAvailable(entity, modelComponentData);
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	model->rotation = rotation;
	shovelerModelUpdateTransformation(model);
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	model->scale = scale;
	shovelerModelUpdateTransformation(model);
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	model->visible = visible;
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	model->emitter = emitter;
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	model->screenspace = screenspace;
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	model->polygonMode = polygonMode;
	return true;
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
	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;

	ShovelerScene *scene = shovelerViewGetScene(view);
	shovelerSceneRemoveModel(scene, model);

	return shovelerViewEntityRemoveComponent(entity, shovelerViewModelComponentName);
}

static ShovelerDrawable *getDrawable(ShovelerView *view, ShovelerViewDrawableConfiguration configuration)
{
	assert(shovelerViewHasDrawables(view));
	ShovelerViewDrawables *drawables = shovelerViewGetDrawables(view);

	switch (configuration.type) {
		case SHOVELER_VIEW_DRAWABLE_TYPE_CUBE:
			return drawables->cube;
			break;
		case SHOVELER_VIEW_DRAWABLE_TYPE_QUAD:
			return drawables->quad;
			break;
		case SHOVELER_VIEW_DRAWABLE_TYPE_POINT:
			return drawables->point;
			break;
		default:
			shovelerLogWarning("Trying to create drawable with unknown type %d, ignoring.", configuration.type);
			return NULL;
	}
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

static void updatePositionIfAvailable(ShovelerViewEntity *entity, ShovelerViewModel *modelComponentData)
{
	ShovelerViewComponent *positionComponent = shovelerViewEntityGetComponent(entity, shovelerViewPositionComponentName);
	if(positionComponent != NULL) {
		positionCallback(positionComponent, SHOVELER_VIEW_COMPONENT_CALLBACK_USER, modelComponentData);
	}
}

static void positionCallback(ShovelerViewComponent *positionComponent, ShovelerViewComponentCallbackType callbackType, void *modelComponentDataPointer)
{
	ShovelerViewPosition *position = positionComponent->data;
	ShovelerViewModel *modelComponentData = modelComponentDataPointer;
	ShovelerModel *model = modelComponentData->model;

	model->translation.values[0] = position->x;
	model->translation.values[1] = position->y;
	model->translation.values[2] = position->z;
	shovelerModelUpdateTransformation(model);
}

static void freeComponent(ShovelerViewComponent *component)
{
	assert(shovelerViewHasScene(component->entity->view));

	ShovelerViewModel *modelComponentData = component->data;

	ShovelerModel *model = modelComponentData->model;
	if(model != NULL) {
		ShovelerScene *scene = shovelerViewGetScene(component->entity->view);
		shovelerSceneRemoveModel(scene, model);
	}

	shovelerViewEntityRemoveCallback(component->entity, shovelerViewPositionComponentName, modelComponentData->positionCallback);

	free(modelComponentData);
}
