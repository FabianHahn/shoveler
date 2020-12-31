#ifndef SHOVELER_SYSTEM_H
#define SHOVELER_SYSTEM_H

#include <shoveler/component.h>

typedef struct ShovelerSystemStruct {
	ShovelerComponentSystemAdapter componentAdapter;
} ShovelerSystem;

ShovelerSystem *shovelerSystemCreate();
ShovelerComponentSystemAdapter *shovelerSystemGetComponentAdapter(ShovelerSystem *system);
void shovelerSystemFree(ShovelerSystem *system);

#endif
