#ifndef SHOVELER_VIEW__H
#define SHOVELER_VIEW__H

#include <stdbool.h> // bool

#include <glib.h>

struct ShovelerViewComponentStruct; // forward declaration

typedef struct {
	/** map from entity id (long long int) to entities (ShovelerViewEntity *) */
	/* private */ GHashTable *entities;
	/** map from string target name to target type */
	/* private */ GHashTable *targets;
	/** map from target (ShovelerViewQualifiedComponent *) to list of (ShovelerViewQualifiedComponent *) */
	/* private */ GHashTable *reverseDependencies;
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
	/* private */ GHashTable *components;
	/** map from string component name to (GQueue *) of (ShovelerViewComponentCallback *) */
	/* private */ GHashTable *callbacks;
} ShovelerViewEntity;

typedef bool (ShovelerViewComponentActivateFunction)(struct ShovelerViewComponentStruct *, void *data);
typedef void (ShovelerViewComponentDeactivateFunction)(struct ShovelerViewComponentStruct *, void *data);
typedef void (ShovelerViewComponentFreeFunction)(struct ShovelerViewComponentStruct *, void *data);

typedef struct ShovelerViewComponentStruct {
	ShovelerViewEntity *entity;
	char *name;
	bool active;
	bool authoritative;
	/* private */ void *data;
	/** list of (ShovelerViewQualifiedComponent *) */
	/* private */ GQueue *dependencies;
	/* private */ ShovelerViewComponentActivateFunction *activate;
	/* private */ ShovelerViewComponentDeactivateFunction *deactivate;
	/* private */ ShovelerViewComponentFreeFunction *free;
} ShovelerViewComponent;

typedef struct {
	long long int entityId;
	char *componentName;
} ShovelerViewQualifiedComponent;

ShovelerView *shovelerViewCreate();
ShovelerViewEntity *shovelerViewAddEntity(ShovelerView *view, long long int entityId);
bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId);
ShovelerViewComponent *shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentName, void *data, ShovelerViewComponentActivateFunction *activate, ShovelerViewComponentDeactivateFunction *deactivate, ShovelerViewComponentFreeFunction *freeFunction);
void shovelerViewComponentUpdate(ShovelerViewComponent *component);
void shovelerViewComponentDelegate(ShovelerViewComponent *component);
void shovelerViewComponentUndelegate(ShovelerViewComponent *component);
/** Adds a new dependency for a component, potentially deactivating it if the dependency isn't active. */
void shovelerViewComponentAddDependency(ShovelerViewComponent *component, long long int dependencyEntityId, const char *dependencyComponentName);
/** Removes an existing dependency from a component, but doesn't automatically activate it even if this was the only unsatisfied one. */
bool shovelerViewComponentRemoveDependency(ShovelerViewComponent *component, long long int dependencyEntityId, const char *dependencyComponentName);
bool shovelerViewComponentActivate(ShovelerViewComponent *component);
void shovelerViewComponentDeactivate(ShovelerViewComponent *component);
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
