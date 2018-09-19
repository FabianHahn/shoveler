#ifndef SHOVELER_VIEW__H
#define SHOVELER_VIEW__H

#include <stdbool.h> // bool

#include <glib.h>

struct ShovelerViewComponentStruct; // forward declaration

typedef struct {
	/** map from entity id (long long int) to entities (ShovelerViewEntity *) */
	GHashTable *entities;
	/** map from string target name to target type */
	GHashTable *targets;
	/** map from target (ShovelerViewQualifiedComponent *) to (ShovelerViewDependency *) */
	GHashTable *dependencies;
} ShovelerView;

typedef enum {
	SHOVELER_VIEW_COMPONENT_CALLBACK_ADD,
	SHOVELER_VIEW_COMPONENT_CALLBACK_REMOVE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_UPDATE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_DELEGATE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_UNDELEGATE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_USER,
} ShovelerViewComponentCallbackType;

typedef void (ShovelerViewComponentCallbackFunction)(struct ShovelerViewComponentStruct *component, ShovelerViewComponentCallbackType callbackType, void *userData);

typedef struct {
	ShovelerViewComponentCallbackFunction *function;
	void *userData;
} ShovelerViewComponentCallback;

typedef struct {
	ShovelerView *view;
	long long int entityId;
	/** map from string component name to (ShovelerViewComponent *) */
	GHashTable *components;
	/** map from string component name to (GQueue *) of (ShovelerViewComponentCallback *) */
	GHashTable *callbacks;
} ShovelerViewEntity;

typedef void (ShovelerViewComponentFreeFunction)(struct ShovelerViewComponentStruct *);

typedef struct ShovelerViewComponentStruct {
	ShovelerViewEntity *entity;
	char *name;
	void *data;
	bool active;
	bool authoritative;
	/** list of (ShovelerViewQualifiedComponent *) */
	GQueue *dependencies;
	ShovelerViewComponentFreeFunction *free;
} ShovelerViewComponent;

typedef struct {
	long long int entityId;
	char *componentName;
} ShovelerViewQualifiedComponent;

typedef struct {
	ShovelerViewQualifiedComponent sourceComponent;
	ShovelerViewQualifiedComponent targetComponent;
} ShovelerViewDependency;

ShovelerView *shovelerViewCreate();
bool shovelerViewAddEntity(ShovelerView *view, long long int entityId);
bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId);
bool shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentName, void *data, ShovelerViewComponentFreeFunction *freeFunction);
bool shovelerViewEntityUpdateComponent(ShovelerViewEntity *entity, const char *componentName);
bool shovelerViewDelegateComponent(ShovelerViewEntity *entity, const char *componentName);
bool shovelerViewUndelegateComponent(ShovelerViewEntity *entity, const char *componentName);
bool shovelerViewEntityAddComponentDependency(ShovelerViewEntity *entity, const char *componentName, long long int dependencyEntityId, const char *dependencyComponentName);
bool shovelerViewEntityRemoveComponent(ShovelerViewEntity *entity, const char *componentName);
ShovelerViewComponentCallback *shovelerViewEntityAddCallback(ShovelerViewEntity *entity, const char *componentName, ShovelerViewComponentCallbackFunction *function, void *userData);
bool shovelerViewEntityRemoveCallback(ShovelerViewEntity *entity, const char *componentName, ShovelerViewComponentCallback *callback);
/** Sets a target that is expected to be freed by the caller. */
bool shovelerViewSetTarget(ShovelerView *view, const char *targetName, void *target);
void shovelerViewFree(ShovelerView *view);

static inline ShovelerViewEntity *shovelerViewGetEntity(ShovelerView *view, long long int entityId)
{
	return (ShovelerViewEntity *) g_hash_table_lookup(view->entities, &entityId);
}

static inline ShovelerViewComponent *shovelerViewEntityGetComponent(ShovelerViewEntity *entity, const char *component)
{
	return (ShovelerViewComponent *) g_hash_table_lookup(entity->components, component);
}

static inline void *shovelerViewGetTarget(ShovelerView *view, const char *targetName)
{
	return g_hash_table_lookup(view->targets, targetName);
}

#endif
