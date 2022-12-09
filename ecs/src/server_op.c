#include "shoveler/server_op.h"

#include <shoveler/component.h>
#include <shoveler/component_type_indexer.h>
#include <shoveler/world.h>
#include <stdlib.h>
#include <string.h>

ShovelerServerOp shovelerServerOp() {
  ShovelerServerOp serverOp;
  shovelerServerOpClear(&serverOp);
  return serverOp;
}

ShovelerServerOpWithData* shovelerServerOpCreateWithData(const ShovelerServerOp* inputServerOp) {
  ShovelerServerOpWithData* serverOp = malloc(sizeof(ShovelerServerOpWithData));
  shovelerServerOpInitWithData(serverOp, inputServerOp);
  return serverOp;
}

void shovelerServerOpInitWithData(
    ShovelerServerOpWithData* serverOp, const ShovelerServerOp* inputServerOp) {
  memset(serverOp, 0, sizeof(ShovelerServerOpWithData));

  if (inputServerOp != NULL) {
    if (inputServerOp->type == SHOVELER_SERVER_OP_UPDATE_COMPONENT) {
      shovelerComponentFieldInitValue(
          &serverOp->fieldValue, inputServerOp->updateComponent.fieldValue->type);
      shovelerComponentFieldAssignValue(
          &serverOp->fieldValue, inputServerOp->updateComponent.fieldValue);
    }

    serverOp->op = *inputServerOp;
    serverOp->op.updateComponent.fieldValue = &serverOp->fieldValue;
  }
}

void shovelerServerOpClear(ShovelerServerOp* serverOp) {
  memset(serverOp, 0, sizeof(ShovelerServerOp));
}

void shovelerServerOpClearWithData(ShovelerServerOpWithData* serverOp) {
  if (serverOp->op.type == SHOVELER_SERVER_OP_UPDATE_COMPONENT) {
    shovelerComponentFieldClearValue(&serverOp->fieldValue);
  }
  memset(serverOp, 0, sizeof(ShovelerServerOpWithData));
}

void shovelerServerOpFreeWithData(ShovelerServerOpWithData* serverOp) {
  shovelerServerOpClearWithData(serverOp);
  free(serverOp);
}

bool shovelerServerOpEquals(const ShovelerServerOp* serverOp1, const ShovelerServerOp* serverOp2) {
  if (serverOp1->type != serverOp2->type) {
    return false;
  }

  switch (serverOp1->type) {
  case SHOVELER_SERVER_OP_NOOP:
    return true;
  case SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST:
    return serverOp1->addEntityInterest.entityId == serverOp2->addEntityInterest.entityId;
  case SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST:
    return serverOp1->removeEntityInterest.entityId == serverOp2->removeEntityInterest.entityId;
  case SHOVELER_SERVER_OP_UPDATE_COMPONENT:
    if (serverOp1->updateComponent.entityId != serverOp2->updateComponent.entityId ||
        serverOp1->updateComponent.componentTypeId != serverOp2->updateComponent.componentTypeId ||
        serverOp1->updateComponent.fieldId != serverOp2->updateComponent.fieldId) {
      return false;
    }
    return shovelerComponentFieldCompareValue(
        serverOp1->updateComponent.fieldValue, serverOp2->updateComponent.fieldValue);
  default:
    return false;
  }
}

bool shovelerServerOpSerialize(
    const ShovelerServerOp* serverOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    GString* output) {
  g_string_append_c(output, (gchar) serverOp->type);
  switch (serverOp->type) {
  case SHOVELER_SERVER_OP_NOOP:
    break;
  case SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST:
    g_string_append_len(
        output,
        (gchar*) &serverOp->addEntityInterest.entityId,
        sizeof(serverOp->addEntityInterest.entityId));
    break;
  case SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST:
    g_string_append_len(
        output,
        (gchar*) &serverOp->removeEntityInterest.entityId,
        sizeof(serverOp->removeEntityInterest.entityId));
    break;
  case SHOVELER_SERVER_OP_UPDATE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &serverOp->updateComponent.entityId,
        sizeof(serverOp->updateComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, serverOp->updateComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    g_string_append_len(
        output,
        (gchar*) &serverOp->updateComponent.fieldId,
        sizeof(serverOp->updateComponent.fieldId));
    if (!shovelerComponentFieldSerializeValue(serverOp->updateComponent.fieldValue, output)) {
      return false;
    }
    break;
  }
  default:
    return false;
  }

  return true;
}

bool shovelerServerOpDeserialize(
    ShovelerServerOpWithData* serverOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    const unsigned char* buffer,
    int bufferSize,
    int* readIndex) {
  shovelerServerOpClearWithData(serverOp);

#define PARSE_VALUE(TARGET, TYPE) \
  if (*readIndex + sizeof(TYPE) > bufferSize) { \
    return false; \
  } \
  memcpy(&(TARGET), &buffer[*readIndex], sizeof(TYPE)); \
  (*readIndex) += sizeof(TYPE)

  char typeChar;
  PARSE_VALUE(typeChar, char);
  if (typeChar < 0 || typeChar > SHOVELER_SERVER_OP_UPDATE_COMPONENT) {
    return false;
  }
  serverOp->op.type = (ShovelerServerOpType) typeChar;

  switch (serverOp->op.type) {
  case SHOVELER_SERVER_OP_NOOP:
    break;
  case SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST:
    PARSE_VALUE(serverOp->op.addEntityInterest.entityId, long long int);
    break;
  case SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST:
    PARSE_VALUE(serverOp->op.removeEntityInterest.entityId, long long int);
    break;
  case SHOVELER_SERVER_OP_UPDATE_COMPONENT: {
    PARSE_VALUE(serverOp->op.updateComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    serverOp->op.updateComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (serverOp->op.updateComponent.componentTypeId == NULL) {
      return false;
    }
    PARSE_VALUE(serverOp->op.updateComponent.fieldId, int);
    if (!shovelerComponentFieldDeserializeValue(
            &serverOp->fieldValue, buffer, bufferSize, readIndex)) {
      return false;
    }
    serverOp->op.updateComponent.fieldValue = &serverOp->fieldValue;
    break;
  }
  default:
    return false;
  }

#undef PARSE_VALUE

  return true;
}

char* shovelerServerOpDebugPrint(const ShovelerServerOp* serverOp) {
  GString* output = g_string_new("");

  switch (serverOp->type) {
  case SHOVELER_SERVER_OP_NOOP:
    g_string_append_printf(output, "Noop()");
    break;
  case SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST:
    g_string_append_printf(output, "AddEntityInterest(%lld)", serverOp->addEntityInterest.entityId);
    break;
  case SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST:
    g_string_append_printf(
        output, "RemoveEntityInterest(%lld)", serverOp->removeEntityInterest.entityId);
    break;
  case SHOVELER_SERVER_OP_UPDATE_COMPONENT:
    g_string_append_printf(
        output,
        "UpdateComponent(%lld, %s:%d)",
        serverOp->updateComponent.entityId,
        serverOp->updateComponent.componentTypeId,
        serverOp->updateComponent.fieldId);
    break;
  default:
    g_string_append_printf(output, "(invalid server op))");
    break;
  }

  char* string = output->str;
  g_string_free(output, /* freeSegment */ false);
  return string;
}

bool shovelerWorldApplyServerOp(ShovelerWorld* world, const ShovelerServerOp* serverOp) {
  switch (serverOp->type) {
  case SHOVELER_SERVER_OP_NOOP:
    return true;
  case SHOVELER_SERVER_OP_ADD_ENTITY_INTEREST: {
    ShovelerWorldEntity* entity =
        shovelerWorldAddEntity(world, serverOp->addEntityInterest.entityId);
    return entity != NULL;
  }
  case SHOVELER_SERVER_OP_REMOVE_ENTITY_INTEREST:
    return shovelerWorldRemoveEntity(world, serverOp->removeEntityInterest.entityId);
  case SHOVELER_SERVER_OP_UPDATE_COMPONENT: {
    ShovelerWorldEntity* entity = shovelerWorldGetEntity(world, serverOp->updateComponent.entityId);
    if (entity == NULL) {
      return false;
    }

    ShovelerComponent* component =
        shovelerWorldEntityGetComponent(entity, serverOp->updateComponent.componentTypeId);
    if (component == NULL) {
      return false;
    }

    return shovelerComponentUpdateField(
        component,
        serverOp->updateComponent.fieldId,
        serverOp->updateComponent.fieldValue,
        /* isAuthoritative */ true);
  }
  default:
    return false;
  }
}
