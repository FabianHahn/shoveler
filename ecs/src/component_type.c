#include "shoveler/component_type.h"

#include <assert.h> // assert
#include <stdlib.h> // malloc free

#include "shoveler/component_field.h"

ShovelerComponentType* shovelerComponentTypeCreate(
    const char* id, int numFields, const ShovelerComponentField* fields) {
  assert(numFields >= 0);

  ShovelerComponentType* componentType = malloc(sizeof(ShovelerComponentType));
  componentType->id = id;
  componentType->numFields = numFields;
  componentType->fields = NULL;

  if (numFields > 0) {
    componentType->fields = malloc((size_t) numFields * sizeof(ShovelerComponentField));
    for (int id = 0; id < numFields; id++) {
      componentType->fields[id] = fields[id];
    }
  }

  return componentType;
}

void shovelerComponentTypeFree(ShovelerComponentType* componentType) {
  if (componentType == NULL) {
    return;
  }

  free(componentType->fields);
  free(componentType);
}
