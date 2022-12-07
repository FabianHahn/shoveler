#ifndef SHOVELER_COMPONENT_FIELD_VALUE_WRAPPER_H
#define SHOVELER_COMPONENT_FIELD_VALUE_WRAPPER_H

extern "C" {
#include <shoveler/component_field.h>
};

// Helper type to allow using ShovelerComponentFieldValue object safely from C++.
struct ShovelerComponentFieldValueWrapper {
  ShovelerComponentFieldValueWrapper(const ShovelerComponentFieldValue* input) {
    shovelerComponentFieldInitValue(&value, input->type);
    shovelerComponentFieldAssignValue(&value, input);
  }

  ShovelerComponentFieldValueWrapper(const ShovelerComponentFieldValueWrapper& other) {
    shovelerComponentFieldInitValue(&value, other.value.type);
    shovelerComponentFieldAssignValue(&value, &other.value);
  }

  ~ShovelerComponentFieldValueWrapper() { shovelerComponentFieldClearValue(&value); }

  ShovelerComponentFieldValueWrapper& operator=(const ShovelerComponentFieldValueWrapper& other) {
    if (&other != this) {
      shovelerComponentFieldAssignValue(&value, &other.value);
    }

    return *this;
  }

  const ShovelerComponentFieldValue& operator*() const { return value; }

  const ShovelerComponentFieldValue* operator->() const { return &value; }

  ShovelerComponentFieldValue value;
};

#endif
