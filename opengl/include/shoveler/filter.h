#ifndef SHOVELER_FILTER_H
#define SHOVELER_FILTER_H

#include <shoveler/render_state.h>
#include <shoveler/texture.h>
#include <shoveler/types.h>
#include <shoveler/uniform_map.h>

struct ShovelerFilterStruct;

typedef int(ShovelerFilterFilteringFunction)(
    struct ShovelerFilterStruct* filter, ShovelerRenderState* renderState);
typedef void(ShovelerFilterFreeDataFunction)(void* data);

typedef struct ShovelerFilterStruct {
  ShovelerTexture* inputTexture;
  ShovelerTexture* outputTexture;
  void* data;
  ShovelerFilterFilteringFunction* filterTexture;
  ShovelerFilterFreeDataFunction* freeData;
} ShovelerFilter;

static inline int shovelerFilterRender(
    ShovelerFilter* filter, ShovelerTexture* inputTexture, ShovelerRenderState* renderState) {
  filter->inputTexture = inputTexture;
  return filter->filterTexture(filter, renderState);
}

static inline void shovelerFilterFree(ShovelerFilter* filter) { filter->freeData(filter->data); }

#endif
