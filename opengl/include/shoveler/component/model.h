#ifndef SHOVELER_COMPONENT_MODEL_H
#define SHOVELER_COMPONENT_MODEL_H

typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

extern const char *const shovelerComponentTypeIdModel;

typedef enum {
	SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW,
	SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE,
} ShovelerComponentModelOptionId;

ShovelerComponentType *shovelerComponentCreateModelType();
ShovelerModel *shovelerComponentGetModel(ShovelerComponent *component);

#endif
