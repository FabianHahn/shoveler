#ifndef SHOVELER_COMPONENT_MODEL_H
#define SHOVELER_COMPONENT_MODEL_H

typedef struct ShovelerModelStruct ShovelerModel; // forward declaration: model.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameModel = "model";
static const char *shovelerComponentModelOptionKeyPosition = "position";
static const char *shovelerComponentModelOptionKeyDrawable = "drawable";
static const char *shovelerComponentModelOptionKeyMaterial = "material";
static const char *shovelerComponentModelOptionKeyRotation = "rotation";
static const char *shovelerComponentModelOptionKeyScale = "scale";
static const char *shovelerComponentModelOptionKeyVisible = "visible";
static const char *shovelerComponentModelOptionKeyEmitter = "emitter";
static const char *shovelerComponentModelOptionKeyCastsShadow = "castsShadow";
static const char *shovelerComponentModelOptionKeyPolygonMode = "polygonMode";

ShovelerComponentType *shovelerComponentCreateModelType();
ShovelerModel *shovelerComponentGetModel(ShovelerComponent *component);

#endif
