#include "shoveler/world.h"

#include <stdlib.h> // malloc free

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentTypeId, void *userData);
static void updateAuthoritativeComponent(ShovelerComponent *component, const ShovelerComponentField *field, const ShovelerComponentFieldValue *value, void *userData);
static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *userData);
static bool removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *userData);
static void forEachReverseDependency(ShovelerComponent *component, ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *adapterUserData);

ShovelerWorld *shovelerWorldCreate()
{
	ShovelerWorld *world = malloc(sizeof(ShovelerWorld));
	world->componentAdapter.getComponent = getComponent;
	world->componentAdapter.updateAuthoritativeComponent = updateAuthoritativeComponent;
	world->componentAdapter.addDependency = addDependency;
	world->componentAdapter.removeDependency = removeDependency;
	world->componentAdapter.forEachReverseDependency = forEachReverseDependency;
	world->componentAdapter.userData = world;

	return world;
}

ShovelerComponentWorldAdapter *shovelerWorldGetComponentAdapter(ShovelerWorld *world)
{
	return &world->componentAdapter;
}

void shovelerWorldFree(ShovelerWorld *world)
{
	free(world);
}

static ShovelerComponent *getComponent(ShovelerComponent *component, long long int entityId, const char *componentTypeId, void *userData)
{
	return NULL;
}

static void updateAuthoritativeComponent(ShovelerComponent *component, const ShovelerComponentField *field, const ShovelerComponentFieldValue *value, void *userData)
{

}

static void addDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *userData)
{

}

static bool removeDependency(ShovelerComponent *component, long long int targetEntityId, const char *targetComponentTypeId, void *userData)
{
	return false;
}

static void forEachReverseDependency(ShovelerComponent *component, ShovelerComponentWorldAdapterForEachReverseDependencyCallbackFunction *callbackFunction, void *callbackUserData, void *adapterUserData)
{

}
