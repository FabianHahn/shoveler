#ifndef SHOVELER_COMPONENT_IMAGE_H
#define SHOVELER_COMPONENT_IMAGE_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerImageStruct ShovelerImage; // forward declaration: image.h

extern const char *const shovelerComponentTypeIdImage;

typedef enum {
	SHOVELER_COMPONENT_IMAGE_FORMAT_PNG,
	SHOVELER_COMPONENT_IMAGE_FORMAT_PPM,
} ShovelerComponentImageFormat;

typedef enum {
	SHOVELER_COMPONENT_IMAGE_OPTION_ID_FORMAT,
	SHOVELER_COMPONENT_IMAGE_OPTION_ID_RESOURCE,
} ShovelerComponentImageOptionId;

ShovelerComponentType *shovelerComponentCreateImageType();
ShovelerImage *shovelerComponentGetImage(ShovelerComponent *component);

#endif
