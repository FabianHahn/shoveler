#ifndef SHOVELER_COMPONENT_DRAWABLE_H
#define SHOVELER_COMPONENT_DRAWABLE_H

typedef struct ShovelerDrawableStruct ShovelerDrawable; // forward declaration: drawable.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdDrawable;

typedef enum {
	SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE,
	SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TILES_WIDTH,
	SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TILES_HEIGHT,
} ShovelerComponentDrawableOptionId;

typedef enum {
	SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE,
	SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD,
	SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT,
	SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES,
} ShovelerComponentDrawableType;

ShovelerComponentType *shovelerComponentCreateDrawableType();
ShovelerDrawable *shovelerComponentGetDrawable(ShovelerComponent *component);

#endif
