#ifndef SHOVELER_COMPONENT_LIGHT_H
#define SHOVELER_COMPONENT_LIGHT_H

typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

typedef enum {
	SHOVELER_COMPONENT_LIGHT_TYPE_SPOT,
	SHOVELER_COMPONENT_LIGHT_TYPE_POINT
} ShovelerComponentLightType;

static const char *shovelerViewLightComponentTypeName = "light";
static const char *shovelerViewLightPositionOptionKey = "position";
static const char *shovelerViewLightTypeOptionKey = "type";
static const char *shovelerViewLightWidthOptionKey = "width";
static const char *shovelerViewLightHeightOptionKey = "height";
static const char *shovelerViewLightSamplesOptionKey = "samples";
static const char *shovelerViewLightAmbientFactorOptionKey = "ambientFactor";
static const char *shovelerViewLightExponentialFactorOptionKey = "exponentialFactor";
static const char *shovelerViewLightColorOptionKey = "color";

ShovelerComponentType *shovelerComponentCreateLightType();
ShovelerLight *shovelerComponentGetLight(ShovelerComponent *component);

#endif
