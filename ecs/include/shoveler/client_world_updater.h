#ifndef SHOVELER_CLIENT_WORLD_UPDATER_H
#define SHOVELER_CLIENT_WORLD_UPDATER_H

typedef struct ShovelerClientOpStruct ShovelerClientOp;
typedef struct ShovelerWorldStruct ShovelerWorld;

typedef struct ShovelerClientWorldUpdater {
  ShovelerWorld* world;
} ShovelerClientWorldUpdater;

typedef enum {
  SHOVELER_CLIENT_WORLD_UPDATER_SUCCESS,
  SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_ALREADY_EXISTS,
  SHOVELER_CLIENT_WORLD_UPDATER_ENTITY_DOESNT_EXIST,
  SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_ALREADY_EXISTS,
  SHOVELER_CLIENT_WORLD_UPDATER_COMPONENT_DOESNT_EXISTS,
  SHOVELER_CLIENT_WORLD_UPDATER_INVALID_COMPONENT_TYPE,
  SHOVELER_CLIENT_WORLD_UPDATER_INVALID_FIELD_TYPE,
  SHOVELER_CLIENT_WORLD_UPDATER_NOT_AUTHORITATIVE,
  SHOVELER_CLIENT_WORLD_UPDATER_DEPENDENCIES_INACTIVE,
  SHOVELER_CLIENT_WORLD_UPDATER_ACTIVATION_FAILURE,
} ShovelerClientWorldUpdaterStatus;

static inline ShovelerClientWorldUpdater shovelerClientWorldUpdater(ShovelerWorld* world) {
  ShovelerClientWorldUpdater clientWorldUpdater;
  clientWorldUpdater.world = world;
  return clientWorldUpdater;
}

ShovelerClientWorldUpdaterStatus shovelerClientWorldUpdaterApplyOp(
    ShovelerClientWorldUpdater* clientWorldUpdater, const ShovelerClientOp* clientOp);
const char* shovelerClientWorldUpdaterStatusToString(ShovelerClientWorldUpdaterStatus status);

#endif
