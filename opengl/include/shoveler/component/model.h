#ifndef SHOVELER_COMPONENT_MODEL_H
#define SHOVELER_COMPONENT_MODEL_H

typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewModelComponentTypeName = "model";
static const char *shovelerViewModelPositionOptionKey = "position";
static const char *shovelerViewModelDrawableOptionKey = "drawable";
static const char *shovelerViewModelMaterialOptionKey = "material";
static const char *shovelerViewModelRotationOptionKey = "rotation";
static const char *shovelerViewModelScaleOptionKey = "scale";
static const char *shovelerViewModelVisibleOptionKey = "visible";
static const char *shovelerViewModelEmitterOptionKey = "emitter";
static const char *shovelerViewModelCastsShadowOptionKey = "castsShadow";
static const char *shovelerViewModelPolygonModeOptionKey = "polygonMode";

ShovelerComponentType *shovelerComponentCreateModelType();
ShovelerModel *shovelerComponentGetModel(ShovelerComponent *component);

#endif
