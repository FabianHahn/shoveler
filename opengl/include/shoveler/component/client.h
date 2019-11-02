#ifndef SHOVELER_COMPONENT_CLIENT_H
#define SHOVELER_COMPONENT_CLIENT_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameClient = "client";
static const char *shovelerComponentClientOptionKeyPosition = "position";
static const char *shovelerComponentClientOptionKeyModel = "model";

ShovelerComponentType *shovelerComponentCreateClientType();

#endif
