#ifndef SHOVELER_DRAWABLE_H
#define SHOVELER_DRAWABLE_H

#include <stdbool.h> // bool

struct ShovelerDrawableStruct;

typedef bool(ShovelerDrawableDrawFunction)(struct ShovelerDrawableStruct* drawable);
typedef void(ShovelerDrawableFreeFunction)(struct ShovelerDrawableStruct* drawable);

typedef struct ShovelerDrawableStruct {
  ShovelerDrawableDrawFunction* draw;
  ShovelerDrawableFreeFunction* free;
  void* data;
} ShovelerDrawable;

static inline bool shovelerDrawableDraw(ShovelerDrawable* drawable) {
  return drawable->draw(drawable);
}

static inline void shovelerDrawableFree(ShovelerDrawable* drawable) { drawable->free(drawable); }

#endif
