#ifndef SHOVELER_TEST_COMPONENT_TYPES_H
#define SHOVELER_TEST_COMPONENT_TYPES_H

#include <shoveler/component_field.h>
#include <shoveler/component_type.h>

static const char* componentType1Id = "component_type_1";
static const char* componentType2Id = "component_type_2";
static const char* componentType3Id = "component_type_3";

static const char* componentType1FieldPrimitive = "primitive";
static const char* componentType1FieldDependencyLiveUpdate = "dependency_live_update";
static const char* componentType1FieldDependencyReactivate = "dependency_reactivate";
static const char* componentType2FieldPrimitiveLiveUpdate = "primitive_live_update";
static const char* componentType3FieldDependency = "dependency";

enum {
  COMPONENT_TYPE_1_FIELD_PRIMITIVE,
  COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE,
  COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE,
};

enum {
  COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE,
};

enum {
  COMPONENT_TYPE_3_FIELD_DEPENDENCY,
};

static inline ShovelerComponentType* shovelerCreateTestComponentType1() {
  ShovelerComponentField componentType1Fields[3];
  componentType1Fields[COMPONENT_TYPE_1_FIELD_PRIMITIVE] = shovelerComponentField(
      componentType1FieldPrimitive,
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  componentType1Fields[COMPONENT_TYPE_1_FIELD_DEPENDENCY_LIVE_UPDATE] =
      shovelerComponentFieldDependency(
          componentType1FieldDependencyLiveUpdate,
          componentType2Id,
          /* isArray */ false,
          /* isOptional */ true);
  componentType1Fields[COMPONENT_TYPE_1_FIELD_DEPENDENCY_REACTIVATE] =
      shovelerComponentFieldDependency(
          componentType1FieldDependencyReactivate,
          componentType2Id,
          /* isArray */ false,
          /* isOptional */ true);

  return shovelerComponentTypeCreate(
      componentType1Id,
      sizeof(componentType1Fields) / sizeof(componentType1Fields[0]),
      componentType1Fields);
}

static inline ShovelerComponentType* shovelerCreateTestComponentType2() {
  ShovelerComponentField componentType2Fields[1];
  componentType2Fields[COMPONENT_TYPE_2_FIELD_PRIMITIVE_LIVE_UPDATE] = shovelerComponentField(
      componentType2FieldPrimitiveLiveUpdate,
      SHOVELER_COMPONENT_FIELD_TYPE_STRING,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      componentType2Id,
      sizeof(componentType2Fields) / sizeof(componentType2Fields[0]),
      componentType2Fields);
}

static inline ShovelerComponentType* shovelerCreateTestComponentType3() {
  ShovelerComponentField componentType3Fields[1];
  componentType3Fields[COMPONENT_TYPE_3_FIELD_DEPENDENCY] = shovelerComponentFieldDependency(
      componentType3FieldDependency,
      componentType1Id,
      /* isArray */ false,
      /* isOptional */ true);

  return shovelerComponentTypeCreate(
      componentType3Id,
      sizeof(componentType3Fields) / sizeof(componentType3Fields[0]),
      componentType3Fields);
}

#endif
