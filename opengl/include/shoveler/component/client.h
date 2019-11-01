#ifndef SHOVELER_COMPONENT_CLIENT_H
#define SHOVELER_COMPONENT_CLIENT_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewClientComponentTypeName = "client";
static const char *shovelerViewClientPositionOptionKey = "position";
static const char *shovelerViewClientModelOptionKey = "model";

ShovelerComponentType *shovelerComponentCreateClientType();

#endif
