#ifndef SHOVELER_COMPONENT_DRAWABLE_H
#define SHOVELER_COMPONENT_DRAWABLE_H

typedef struct ShovelerDrawableStruct ShovelerDrawable; // forward declaration: drawable.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

typedef enum {
	SHOVELER_COMPONENT_DRAWABLE_TYPE_CUBE,
	SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD,
	SHOVELER_COMPONENT_DRAWABLE_TYPE_POINT,
	SHOVELER_COMPONENT_DRAWABLE_TYPE_TILES,
} ShovelerComponentDrawableType;

extern const char *const shovelerComponentTypeIdDrawable;
static const char *shovelerComponentDrawableOptionKeyType = "type";
static const char *shovelerComponentDrawableOptionKeyTilesWidth = "tiles_width";
static const char *shovelerComponentDrawableOptionKeyTilesHeight = "tiles_height";

ShovelerComponentType *shovelerComponentCreateDrawableType();
ShovelerDrawable *shovelerComponentGetDrawable(ShovelerComponent *component);

#endif
