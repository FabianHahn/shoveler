#ifndef SHOVELER_COMPONENT_TYPE_H
#define SHOVELER_COMPONENT_TYPE_H

typedef struct ShovelerComponentFieldStruct
    ShovelerComponentField; // forward declaration: component_field.h

typedef struct ShovelerComponentTypeStruct {
  const char* id;
  int numFields;
  ShovelerComponentField* fields;
} ShovelerComponentType;

/**
 * Creates a new component type.
 *
 * The specified ID should be a statically defined string with external linkage that will be used
 * as unique identifier for the component type. It is a string so it can be easily printed as part
 * of log messages.
 *
 * A component type can be created with any number of fields that will be instantiated on each
 * component instance of this type. The caller retains ownership of the passed fields.
 */
ShovelerComponentType* shovelerComponentTypeCreate(
    const char* id, int numFields, const ShovelerComponentField* fields);
void shovelerComponentTypeFree(ShovelerComponentType* componentType);

#endif
