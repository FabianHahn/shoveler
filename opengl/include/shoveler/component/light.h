#ifndef SHOVELER_COMPONENT_LIGHT_H
#define SHOVELER_COMPONENT_LIGHT_H

typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdLight;

typedef enum {
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_POSITION,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_TYPE,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_WIDTH,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_HEIGHT,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_SAMPLES,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_AMBIENT_FACTOR,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_EXPONENTIAL_FACTOR,
	SHOVELER_COMPONENT_LIGHT_OPTION_ID_COLOR,
} ShovelerComponentLightOptionId;

typedef enum {
	SHOVELER_COMPONENT_LIGHT_TYPE_SPOT,
	SHOVELER_COMPONENT_LIGHT_TYPE_POINT
} ShovelerComponentLightType;

ShovelerComponentType *shovelerComponentCreateLightType();
ShovelerLight *shovelerComponentGetLight(ShovelerComponent *component);

#endif
