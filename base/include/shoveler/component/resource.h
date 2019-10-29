#ifndef SHOVELER_COMPONENT_RESOURCE_H
#define SHOVELER_COMPONENT_RESOURCE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameResource = "resource";
static const char *shovelerComponentResourceOptionKeyTypeId = "type_id";
static const char *shovelerComponentResourceOptionKeyBuffer = "buffer";

ShovelerComponentType *shovelerComponentCreateResourceType();
void *shovelerComponentGetResource(ShovelerComponent *component);

#endif
