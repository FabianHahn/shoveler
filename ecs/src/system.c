#include "shoveler/system.h"

#include <stdlib.h> // malloc free

static bool requiresAuthority(ShovelerComponent *component, void *userData);
static bool canLiveUpdateField(ShovelerComponent *component, const ShovelerComponentField *field, void *userData);
static bool canLiveUpdateDependencyField(ShovelerComponent *component, const ShovelerComponentField *field, void *userData);
static bool liveUpdateField(ShovelerComponent *component, const ShovelerComponentField *field, ShovelerComponentFieldValue *fieldValue, void *userData);
static bool liveUpdateDependencyField(ShovelerComponent *component, const ShovelerComponentField *field, ShovelerComponent *dependencyComponent, void *userData);
static void *activateSystem(ShovelerComponent *component, void *userData);
static bool updateSystem(ShovelerComponent *component, double dt, void *userData);
static void deactivateSystem(ShovelerComponent *component, void *userData);

ShovelerSystem *shovelerSystemCreate()
{
	ShovelerSystem *system = malloc(sizeof(ShovelerSystem));
	system->componentAdapter.requiresAuthority = requiresAuthority;
	system->componentAdapter.canLiveUpdateField = canLiveUpdateField;
	system->componentAdapter.canLiveUpdateDependencyField = canLiveUpdateDependencyField;
	system->componentAdapter.liveUpdateField = liveUpdateField;
	system->componentAdapter.liveUpdateDependencyField = liveUpdateDependencyField;
	system->componentAdapter.activateSystem = activateSystem;
	system->componentAdapter.updateSystem = updateSystem;
	system->componentAdapter.deactivateSystem = deactivateSystem;
	system->componentAdapter.userData = system;

	return system;
}

ShovelerComponentSystemAdapter *shovelerSystemGetComponentAdapter(ShovelerSystem *system)
{
	return &system->componentAdapter;
}

void shovelerSystemFree(ShovelerSystem *system)
{
	free(system);
}

static bool requiresAuthority(ShovelerComponent *component, void *userData)
{
	return false;
}

static bool canLiveUpdateField(ShovelerComponent *component, const ShovelerComponentField *field, void *userData)
{
	return false;
}

static bool canLiveUpdateDependencyField(ShovelerComponent *component, const ShovelerComponentField *field, void *userData)
{
	return false;
}

static bool liveUpdateField(ShovelerComponent *component, const ShovelerComponentField *field, ShovelerComponentFieldValue *fieldValue, void *userData)
{
	return false;
}

static bool liveUpdateDependencyField(ShovelerComponent *component, const ShovelerComponentField *field, ShovelerComponent *dependencyComponent, void *userData)
{
	return false;
}

static void *activateSystem(ShovelerComponent *component, void *userData)
{
	return component;
}

static bool updateSystem(ShovelerComponent *component, double dt, void *userData)
{
	return false;
}

static void deactivateSystem(ShovelerComponent *component, void *userData)
{

}
