#ifndef SHOVELER_COMPONENT_CLIENT_H
#define SHOVELER_COMPONENT_CLIENT_H

#include <assert.h>
#include <shoveler/component.h>
#include <shoveler/component_type.h>
#include <shoveler/schema/opengl.h>

typedef struct ShovelerClientSystemStruct ShovelerClientSystem;

void shovelerClientSystemAddClientSystem(ShovelerClientSystem* clientSystem);

#endif
