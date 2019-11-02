#ifndef SHOVELER_COMPONENT_RESOURCE_H
#define SHOVELER_COMPONENT_RESOURCE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdResource;

typedef enum {
	SHOVELER_COMPONENT_RESOURCE_OPTION_TYPE_ID,
	SHOVELER_COMPONENT_RESOURCE_OPTION_BUFFER,
} ShovelerComponentPositionOptionId;

ShovelerComponentType *shovelerComponentCreateResourceType();
void *shovelerComponentGetResource(ShovelerComponent *component);

#endif
