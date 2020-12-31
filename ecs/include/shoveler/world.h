#ifndef SHOVELER_WORLD_H
#define SHOVELER_WORLD_H

#include <shoveler/component.h>

typedef struct ShovelerWorldStruct {
	ShovelerComponentWorldAdapter componentAdapter;
} ShovelerWorld;

ShovelerWorld *shovelerWorldCreate();
ShovelerComponentWorldAdapter *shovelerWorldGetComponentAdapter(ShovelerWorld *world);
void shovelerWorldFree(ShovelerWorld *world);

#endif
