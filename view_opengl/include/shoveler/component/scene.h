#ifndef SHOVELER_COMPONENT_SCENE_H
#define SHOVELER_COMPONENT_SCENE_H

#include <stdbool.h> // bool
#include <stddef.h> // NULL

typedef struct ShovelerSceneStruct ShovelerScene; // forward declaration: scene.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentViewTargetIdScene;

ShovelerScene *shovelerComponentGetViewScene(ShovelerComponent *component);

static inline bool shovelerComponentHasViewScene(ShovelerComponent *component)
{
	return shovelerComponentGetViewScene(component) != NULL;
}

#endif
