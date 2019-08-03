#ifndef SHOVELER_VIEW__H
#define SHOVELER_VIEW__H

#include <stdbool.h> // bool

#include <glib.h>

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerViewComponentStruct ShovelerViewComponent; // forward declaration: below

typedef struct ShovelerViewStruct{
	/** map from string component type name to (ShovelerComponentType *) */
	/* private */ GHashTable *componentTypes;
	/** map from entity id (long long int) to entities (ShovelerViewEntity *) */
	/* private */ GHashTable *entities;
	/** map from string target name to target type */
	/* private */ GHashTable *targets;
	/** map from target (ShovelerViewQualifiedComponent *) to list of (ShovelerViewQualifiedComponent *) */
	/* private */ GHashTable *reverseDependencies;
	/** list of (ShovelerViewDependencyCallback *) */
	/* private */ GQueue *dependencyCallbacks;
	unsigned int numEntities;
	unsigned int numComponents;
	unsigned int numComponentDependencies;
	unsigned int numActiveComponents;
	unsigned int numDelegatedComponents;
} ShovelerView;

typedef enum {
	SHOVELER_VIEW_COMPONENT_CALLBACK_ADD,
	SHOVELER_VIEW_COMPONENT_CALLBACK_REMOVE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_UPDATE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_DELEGATE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_UNDELEGATE,
	SHOVELER_VIEW_COMPONENT_CALLBACK_USER,
} ShovelerViewComponentCallbackType;

typedef void (ShovelerViewComponentCallbackFunction)(ShovelerViewComponent *component, ShovelerViewComponentCallbackType callbackType, void *userData);

typedef struct ShovelerViewComponentCallbackStruct {
	ShovelerViewComponentCallbackFunction *function;
	void *userData;
} ShovelerViewComponentCallback;

typedef struct ShovelerViewEntityStruct {
	ShovelerView *view;
	long long int entityId;
	char *type;
	/** map from string component name to (ShovelerViewComponent *) */
	/* private */ GHashTable *components;
	/** map from string component name to (GQueue *) of (ShovelerViewComponentCallback *) */
	/* private */ GHashTable *callbacks;
} ShovelerViewEntity;

typedef bool (ShovelerViewComponentActivateFunction)(ShovelerViewComponent *, void *data);
typedef void (ShovelerViewComponentDeactivateFunction)(ShovelerViewComponent *, void *data);
typedef void (ShovelerViewComponentFreeFunction)(ShovelerViewComponent *, void *data);

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

typedef struct ShovelerViewQualifiedComponentStruct {
	long long int entityId;
	char *componentName;
} ShovelerViewQualifiedComponent;

typedef void (ShovelerViewDependencyCallbackFunction)(ShovelerView *view, const ShovelerViewQualifiedComponent *dependencySource, const ShovelerViewQualifiedComponent *dependencyTarget, bool added, void *userData);

typedef struct ShovelerViewDependencyCallbackStruct {
	ShovelerViewDependencyCallbackFunction *function;
	void *userData;
} ShovelerViewDependencyCallback;

ShovelerView *shovelerViewCreate();
/** Adds a component type to the view, with the view taking ownership over it. */
bool shovelerViewAddComponentType(ShovelerView *view, ShovelerComponentType *componentType);
bool shovelerViewHasComponentType(ShovelerView *view, const char *componentTypeName);
ShovelerViewEntity *shovelerViewAddEntity(ShovelerView *view, long long int entityId);
bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId);
/** Sets an entity's type, which is an arbitrary string without semantic meaning. */
void shovelerViewEntitySetType(ShovelerViewEntity *entity, const char *type);
ShovelerViewComponent *shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentName, void *data, ShovelerViewComponentActivateFunction *activate, ShovelerViewComponentDeactivateFunction *deactivate, ShovelerViewComponentFreeFunction *freeFunction);
ShovelerViewComponent *shovelerViewEntityAddTypedComponent(ShovelerViewEntity *entity, const char *componentTypeName);
ShovelerComponent *shovelerViewEntityGetTypedComponent(ShovelerViewEntity *entity, const char *componentTypeName);
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
/** Creates the view's dependency graph to as a graphviz dot digraph string. */
GString *shovelerViewCreateDependencyGraph(ShovelerView *view);
/** Writes the view's dependency graph to a graphviz dot file. */
bool shovelerViewWriteDependencyGraph(ShovelerView *view, const char *filename);
ShovelerViewDependencyCallback *shovelerViewAddDependencyCallback(ShovelerView *view, ShovelerViewDependencyCallbackFunction *function, void *userData);
bool shovelerViewRemoveDependencyCallback(ShovelerView *view, ShovelerViewDependencyCallback *callback);
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
