#ifndef SHOVELER_SCHEMA_BASE_H
#define SHOVELER_SCHEMA_BASE_H

typedef struct ShovelerSchemaStruct ShovelerSchema;

extern const char* const shovelerComponentTypeIdFont;
extern const char* const shovelerComponentTypeIdFontAtlas;
extern const char* const shovelerComponentTypeIdImage;
extern const char* const shovelerComponentTypeIdPosition;
extern const char* const shovelerComponentTypeIdResource;

typedef enum {
  SHOVELER_COMPONENT_FONT_FIELD_ID_NAME,
  SHOVELER_COMPONENT_FONT_FIELD_ID_RESOURCE,
} ShovelerComponentFontFieldId;

typedef enum {
  SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT,
  SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_FONT_SIZE,
  SHOVELER_COMPONENT_FONT_ATLAS_FIELD_ID_PADDING,
} ShovelerComponentFontAtlasFieldId;

typedef enum {
  SHOVELER_COMPONENT_IMAGE_FORMAT_PNG,
  SHOVELER_COMPONENT_IMAGE_FORMAT_PPM,
} ShovelerComponentImageFormat;

typedef enum {
  SHOVELER_COMPONENT_IMAGE_FIELD_ID_FORMAT,
  SHOVELER_COMPONENT_IMAGE_FIELD_ID_RESOURCE,
} ShovelerComponentImageFieldId;

typedef enum {
  SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE,
  SHOVELER_COMPONENT_POSITION_TYPE_RELATIVE,
} ShovelerComponentPositionType;

typedef enum {
  SHOVELER_COMPONENT_POSITION_FIELD_ID_TYPE,
  SHOVELER_COMPONENT_POSITION_FIELD_ID_COORDINATES,
  SHOVELER_COMPONENT_POSITION_FIELD_ID_RELATIVE_PARENT_POSITION,
} ShovelerComponentPositionFieldId;

typedef enum {
  SHOVELER_COMPONENT_RESOURCE_FIELD_ID_BUFFER,
} ShovelerComponentResourceFieldId;

// Registers all base schema component types in the passed schema.
//
// Must not have registered any of these types before.
void shovelerSchemaBaseRegister(ShovelerSchema* schema);

#endif
