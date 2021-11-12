#ifndef SHOVELER_COMPONENT_CLIENT_H
#define SHOVELER_COMPONENT_CLIENT_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdClient;

typedef enum {
	SHOVELER_COMPONENT_CLIENT_OPTION_ID_POSITION,
	SHOVELER_COMPONENT_CLIENT_OPTION_ID_MODEL,
} ShovelerComponentClientOptionId;

ShovelerComponentType *shovelerComponentCreateClientType();

#endif
