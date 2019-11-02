#ifndef SHOVELER_VIEW_H
#define SHOVELER_VIEW_H

#include <stdbool.h> // bool

#include <glib.h>

typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentViewAdapterStruct ShovelerComponentViewAdapter; // forward declaration: component.h

typedef struct ShovelerViewStruct {
	/** map from string component type name to (ShovelerComponentType *) */
	/* private */ GHashTable *componentTypes;
	/** map from entity id (long long int) to entities (ShovelerViewEntity *) */
	/* private */ GHashTable *entities;
	/** map from string target name to target type */
	/* private */ GHashTable *targets;
	/** map from source (ShovelerViewQualifiedComponent *) to list of (ShovelerViewQualifiedComponent *) */
	/* private */ GHashTable *dependencies;
	/** map from target (ShovelerViewQualifiedComponent *) to list of (ShovelerViewQualifiedComponent *) */
	/* private */ GHashTable *reverseDependencies;
	ShovelerComponentViewAdapter *adapter;
	int numEntities;
	int numComponents;
	int numComponentDependencies;
	int numActiveComponents;
	int numDelegatedComponents;
} ShovelerView;

typedef struct ShovelerViewEntityStruct {
	ShovelerView *view;
	long long int id;
	char *type;
	/** map from string component type name to (ShovelerComponent *) */
	/* private */ GHashTable *components;
	/** set of component type names */
	/* private */ GHashTable *authoritativeComponents;
} ShovelerViewEntity;

typedef struct ShovelerViewQualifiedComponentStruct {
	long long int entityId;
	const char *componentTypeId;
} ShovelerViewQualifiedComponent;

ShovelerView *shovelerViewCreate();
/** Adds a component type to the view, with the view taking ownership over it. */
bool shovelerViewAddComponentType(ShovelerView *view, ShovelerComponentType *componentType);
ShovelerComponentType *shovelerViewGetComponentType(ShovelerView *view, const char *componentTypeId);
/** Adds a new dependency for a component, potentially deactivating it if the dependency isn't active. */
void shovelerViewAddDependency(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeId, long long int targetEntityId, const char *targetComponentTypeId);
/** Removes an existing dependency from a component, but doesn't automatically activate it even if this was the only unsatisfied one. */
bool shovelerViewRemoveDependency(ShovelerView *view, long long int sourceEntityId, const char *sourceComponentTypeId, long long int targetEntityId, const char *targetComponentTypeId);
ShovelerViewEntity *shovelerViewAddEntity(ShovelerView *view, long long int entityId);
bool shovelerViewRemoveEntity(ShovelerView *view, long long int entityId);
void shovelerViewEntityDelegate(ShovelerViewEntity *entity, const char *componentTypeId);
bool shovelerViewEntityIsAuthoritative(ShovelerViewEntity *entity, const char *componentTypeId);
void shovelerViewEntityUndelegate(ShovelerViewEntity *entity, const char *componentTypeId);
/** Sets an entity's type, which is an arbitrary string without semantic meaning. */
void shovelerViewEntitySetType(ShovelerViewEntity *entity, const char *type);
ShovelerComponent *shovelerViewEntityAddComponent(ShovelerViewEntity *entity, const char *componentTypeId);
bool shovelerViewEntityRemoveComponent(ShovelerViewEntity *entity, const char *componentTypeId);
/** Sets a target that is expected to be freed by the caller. */
bool shovelerViewSetTarget(ShovelerView *view, const char *targetName, void *target);
/** Creates the view's dependency graph to as a graphviz dot digraph string. */
GString *shovelerViewCreateDependencyGraph(ShovelerView *view);
/** Writes the view's dependency graph to a graphviz dot file. */
bool shovelerViewWriteDependencyGraph(ShovelerView *view, const char *filename);
void shovelerViewFree(ShovelerView *view);

static inline bool shovelerViewHasComponentType(ShovelerView *view, const char *componentTypeId)
{
	return shovelerViewGetComponentType(view, componentTypeId) != NULL;
}

static inline ShovelerViewEntity *shovelerViewGetEntity(ShovelerView *view, long long int entityId)
{
	return (ShovelerViewEntity *) g_hash_table_lookup(view->entities, &entityId);
}

static inline ShovelerComponent *shovelerViewEntityGetComponent(ShovelerViewEntity *entity, const char *componentTypeId)
{
	return (ShovelerComponent *) g_hash_table_lookup(entity->components, componentTypeId);
}

static inline void *shovelerViewGetTarget(ShovelerView *view, const char *targetName)
{
	return g_hash_table_lookup(view->targets, targetName);
}

#endif
