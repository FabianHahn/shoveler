#ifndef SHOVELER_POSITION_QUANTIZER_H
#define SHOVELER_POSITION_QUANTIZER_H

#include <shoveler/types.h>
#include <stdint.h>

typedef struct ShovelerPositionQuantizerStruct {
  ShovelerVector2 worldMin;
  ShovelerVector2 worldSize;
} ShovelerPositionQuantizer;

static inline ShovelerPositionQuantizer shovelerPositionQuantizer(
    ShovelerVector2 worldMin, ShovelerVector2 worldSize) {
  ShovelerPositionQuantizer positionQuantizer;
  positionQuantizer.worldMin = worldMin;
  positionQuantizer.worldSize = worldSize;
  return positionQuantizer;
}

static inline float shovelerPositionQuantizerToWorldX(
    const ShovelerPositionQuantizer* positionQuantizer, uint64_t quantizedX) {
  float ratioX = (float) quantizedX / (float) UINT64_MAX;
  float diffX = ratioX * positionQuantizer->worldSize.values[0];
  return positionQuantizer->worldMin.values[0] + diffX;
}

static inline float shovelerPositionQuantizerToWorldY(
    const ShovelerPositionQuantizer* positionQuantizer, uint64_t quantizedY) {
  float ratioY = (float) quantizedY / (float) UINT64_MAX;
  float diffY = ratioY * positionQuantizer->worldSize.values[1];
  return positionQuantizer->worldMin.values[1] + diffY;
}

static inline ShovelerVector2 shovelerPositionQuantizerToWorld(
    const ShovelerPositionQuantizer* positionQuantizer, uint64_t quantizedX, uint64_t quantizedY) {
  return shovelerVector2(
      shovelerPositionQuantizerToWorldX(positionQuantizer, quantizedX),
      shovelerPositionQuantizerToWorldY(positionQuantizer, quantizedY));
}

static inline uint64_t shovelerPositionQuantizerFromWorldX(
    const ShovelerPositionQuantizer* positionQuantizer, float worldX) {
  float diffX =
      (worldX - positionQuantizer->worldMin.values[0]) / positionQuantizer->worldSize.values[0];

  if (diffX <= 0.0f) {
    return 0;
  } else if (diffX >= 1.0f) {
    return UINT64_MAX;
  } else {
    return diffX * UINT64_MAX;
  }
}

static inline uint64_t shovelerPositionQuantizerFromWorldY(
    const ShovelerPositionQuantizer* positionQuantizer, float worldY) {
  float diffY =
      (worldY - positionQuantizer->worldMin.values[1]) / positionQuantizer->worldSize.values[1];

  if (diffY <= 0.0f) {
    return 0;
  } else if (diffY >= 1.0f) {
    return UINT64_MAX;
  } else {
    return diffY * UINT64_MAX;
  }
}

static inline void shovelerPositionQuantizerFromWorld(
    const ShovelerPositionQuantizer* positionQuantizer,
    ShovelerVector2 world,
    uint64_t* outputQuantizedX,
    uint64_t* outputQuantizedY) {
  *outputQuantizedX = shovelerPositionQuantizerFromWorldX(positionQuantizer, world.values[0]);
  *outputQuantizedY = shovelerPositionQuantizerFromWorldY(positionQuantizer, world.values[1]);
}

#endif
