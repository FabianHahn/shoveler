#ifndef SHOVELER_SYSTEM_H
#define SHOVELER_SYSTEM_H

#include <glib.h>

typedef struct ShovelerComponentSystemStruct ShovelerComponentSystem;
typedef struct ShovelerComponentTypeStruct ShovelerComponentType;

typedef struct ShovelerSystemStruct {
  /** map from string component type id to (ShovelerComponentSystem *) */
  GHashTable* componentSystems;
  int numActiveComponents;
} ShovelerSystem;

ShovelerSystem* shovelerSystemCreate();
ShovelerComponentSystem* shovelerSystemForComponentType(
    ShovelerSystem* system, ShovelerComponentType* componentType);
void shovelerSystemFree(ShovelerSystem* system);

#endif
