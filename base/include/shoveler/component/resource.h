#ifndef SHOVELER_COMPONENT_RESOURCE_H
#define SHOVELER_COMPONENT_RESOURCE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdResource;

typedef enum {
	SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER,
} ShovelerComponentResourceOptionId;

ShovelerComponentType *shovelerComponentCreateResourceType();
void shovelerComponentGetResource(ShovelerComponent *component, const unsigned char **outputData, int *outputSize);

#endif
