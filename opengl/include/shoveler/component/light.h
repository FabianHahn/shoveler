#ifndef SHOVELER_COMPONENT_LIGHT_H
#define SHOVELER_COMPONENT_LIGHT_H

typedef struct ShovelerLightStruct ShovelerLight; // forward declaration: light.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

typedef enum {
	SHOVELER_COMPONENT_LIGHT_TYPE_SPOT,
	SHOVELER_COMPONENT_LIGHT_TYPE_POINT
} ShovelerComponentLightType;

static const char *shovelerComponentTypeNameLight = "light";
static const char *shovelerComponentLightOptionKeyPosition = "position";
static const char *shovelerComponentLightOptionKeyType = "type";
static const char *shovelerComponentLightOptionKeyWidth = "width";
static const char *shovelerComponentLightOptionKeyHeight = "height";
static const char *shovelerComponentLightOptionKeySamples = "samples";
static const char *shovelerComponentLightOptionKeyAmbientFactor = "ambientFactor";
static const char *shovelerComponentLightOptionKeyExponentialFactor = "exponentialFactor";
static const char *shovelerComponentLightOptionKeyColor = "color";

ShovelerComponentType *shovelerComponentCreateLightType();
ShovelerLight *shovelerComponentGetLight(ShovelerComponent *component);

#endif
