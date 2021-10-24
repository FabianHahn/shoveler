#include "shoveler/schema/base.h"

#include <assert.h>

#include "shoveler/component_field.h"
#include "shoveler/component_type.h"
#include "shoveler/schema.h"

const char* const shovelerComponentTypeIdFont = "font";
const char* const shovelerComponentTypeIdFontAtlas = "font_atlas";
const char* const shovelerComponentTypeIdImage = "image";
const char* const shovelerComponentTypeIdPosition = "position";
const char* const shovelerComponentTypeIdResource = "resource";

static ShovelerComponentType* shovelerComponentCreateFontType();
static ShovelerComponentType* shovelerComponentCreateFontAtlasType();
static ShovelerComponentType* shovelerComponentCreateImageType();
static ShovelerComponentType* shovelerComponentCreatePositionType();
static ShovelerComponentType* shovelerComponentCreateResourceType();

void shovelerSchemaBaseRegister(ShovelerSchema* schema) {
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateFontType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateFontAtlasType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateImageType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreatePositionType());
  shovelerSchemaAddComponentType(schema, shovelerComponentCreateResourceType());
}

static ShovelerComponentType* shovelerComponentCreateFontType() {
  ShovelerComponentField fields[2];
  fields[SHOVELER_COMPONENT_FONT_FIELD_ID_NAME] = shovelerComponentField(
      "name",
      SHOVELER_COMPONENT_FIELD_TYPE_STRING,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_FONT_FIELD_ID_RESOURCE] = shovelerComponentFieldDependency(
      "resource",
      shovelerComponentTypeIdResource,
      /* isArray */ false,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdFont, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateFontAtlasType() {
  ShovelerComponentField fields[3];
  fields[SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT] = shovelerComponentFieldDependency(
      "font",
      shovelerComponentTypeIdFont,
      /* isArray */ false,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT_SIZE] = shovelerComponentField(
      "font_size",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_PADDING] = shovelerComponentField(
      "name",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdFontAtlas, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateImageType() {
  ShovelerComponentField fields[2];
  fields[SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT] = shovelerComponentField(
      "format",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE] = shovelerComponentFieldDependency(
      "resource",
      shovelerComponentTypeIdResource,
      /* isArray */ false,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdImage, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreatePositionType() {
  ShovelerComponentField fields[3];
  fields[SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE] = shovelerComponentField(
      "type",
      SHOVELER_COMPONENT_FIELD_TYPE_INT,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES] = shovelerComponentField(
      "coordinates",
      SHOVELER_COMPONENT_FIELD_TYPE_VECTOR3,
      /* isOptional */ false);
  fields[SHOVELER_COMPONENT_POSITION_FIELD_ID_RELATIVE_PARENT_POSITION] =
      shovelerComponentFieldDependency(
          "relative_parent_position",
          shovelerComponentTypeIdPosition,
          /* isArray */ false,
          /* isOptional */ true);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdPosition, sizeof(fields) / sizeof(fields[0]), fields);
}

static ShovelerComponentType* shovelerComponentCreateResourceType() {
  ShovelerComponentField fields[1];
  fields[SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER] = shovelerComponentField(
      "buffer",
      SHOVELER_COMPONENT_FIELD_TYPE_BYTES,
      /* isOptional */ false);

  return shovelerComponentTypeCreate(
      shovelerComponentTypeIdResource, sizeof(fields) / sizeof(fields[0]), fields);
}
