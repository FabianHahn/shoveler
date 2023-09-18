#include "shoveler/client_op.h"

#include <shoveler/component.h>
#include <shoveler/component_type_indexer.h>
#include <shoveler/world.h>
#include <stdlib.h>
#include <string.h>

ShovelerClientOp shovelerClientOp() {
  ShovelerClientOp clientOp;
  shovelerClientOpClear(&clientOp);
  return clientOp;
}

ShovelerClientOpWithData* shovelerClientOpCreateWithData(const ShovelerClientOp* inputClientOp) {
  ShovelerClientOpWithData* clientOp = malloc(sizeof(ShovelerClientOpWithData));
  shovelerClientOpInitWithData(clientOp, inputClientOp);
  return clientOp;
}

void shovelerClientOpInitWithData(
    ShovelerClientOpWithData* clientOp, const ShovelerClientOp* inputClientOp) {
  memset(clientOp, 0, sizeof(ShovelerClientOpWithData));

  if (inputClientOp != NULL) {
    if (inputClientOp->type == SHOVELER_CLIENT_OP_UPDATE_COMPONENT) {
      shovelerComponentFieldInitValue(
          &clientOp->fieldValue, inputClientOp->updateComponent.fieldValue->type);
      shovelerComponentFieldAssignValue(
          &clientOp->fieldValue, inputClientOp->updateComponent.fieldValue);
    }

    clientOp->op = *inputClientOp;
    clientOp->op.updateComponent.fieldValue = &clientOp->fieldValue;
  }
}

void shovelerClientOpClear(ShovelerClientOp* clientOp) {
  memset(clientOp, 0, sizeof(ShovelerClientOp));
}

void shovelerClientOpClearWithData(ShovelerClientOpWithData* clientOp) {
  if (clientOp->op.type == SHOVELER_CLIENT_OP_UPDATE_COMPONENT) {
    shovelerComponentFieldClearValue(&clientOp->fieldValue);
  }
  memset(clientOp, 0, sizeof(ShovelerClientOpWithData));
}

void shovelerClientOpFreeWithData(ShovelerClientOpWithData* clientOp) {
  shovelerClientOpClearWithData(clientOp);
  free(clientOp);
}

bool shovelerClientOpEquals(const ShovelerClientOp* clientOp1, const ShovelerClientOp* clientOp2) {
  if (clientOp1->type != clientOp2->type) {
    return false;
  }

  switch (clientOp1->type) {
  case SHOVELER_CLIENT_OP_NOOP:
    return true;
  case SHOVELER_CLIENT_OP_ADD_ENTITY:
    return clientOp1->addEntity.entityId == clientOp2->addEntity.entityId;
  case SHOVELER_CLIENT_OP_REMOVE_ENTITY:
    return clientOp1->removeEntity.entityId == clientOp2->removeEntity.entityId;
  case SHOVELER_CLIENT_OP_ADD_COMPONENT:
    return clientOp1->addComponent.entityId == clientOp2->addComponent.entityId &&
        clientOp1->addComponent.componentTypeId == clientOp2->addComponent.componentTypeId;
  case SHOVELER_CLIENT_OP_UPDATE_COMPONENT:
    if (clientOp1->updateComponent.entityId != clientOp2->updateComponent.entityId ||
        clientOp1->updateComponent.componentTypeId != clientOp2->updateComponent.componentTypeId ||
        clientOp1->updateComponent.fieldId != clientOp2->updateComponent.fieldId) {
      return false;
    }
    return shovelerComponentFieldCompareValue(
        clientOp1->updateComponent.fieldValue, clientOp2->updateComponent.fieldValue);
  case SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT:
    return clientOp1->activateComponent.entityId == clientOp2->activateComponent.entityId &&
        clientOp1->activateComponent.componentTypeId ==
        clientOp2->activateComponent.componentTypeId;
  case SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT:
    return clientOp1->deactivateComponent.entityId == clientOp2->deactivateComponent.entityId &&
        clientOp1->deactivateComponent.componentTypeId ==
        clientOp2->deactivateComponent.componentTypeId;
  case SHOVELER_CLIENT_OP_DELEGATE_COMPONENT:
    return clientOp1->delegateComponent.entityId == clientOp2->delegateComponent.entityId &&
        clientOp1->delegateComponent.componentTypeId ==
        clientOp2->delegateComponent.componentTypeId;
  case SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT:
    return clientOp1->undelegateComponent.entityId == clientOp2->undelegateComponent.entityId &&
        clientOp1->undelegateComponent.componentTypeId ==
        clientOp2->undelegateComponent.componentTypeId;
  case SHOVELER_CLIENT_OP_REMOVE_COMPONENT:
    return clientOp1->removeComponent.entityId == clientOp2->removeComponent.entityId &&
        clientOp1->removeComponent.componentTypeId == clientOp2->removeComponent.componentTypeId;
  default:
    return false;
  }
}

bool shovelerClientOpSerialize(
    const ShovelerClientOp* clientOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    GString* output) {
  g_string_append_c(output, (gchar) clientOp->type);
  switch (clientOp->type) {
  case SHOVELER_CLIENT_OP_NOOP:
    break;
  case SHOVELER_CLIENT_OP_ADD_ENTITY:
    g_string_append_len(
        output, (gchar*) &clientOp->addEntity.entityId, sizeof(clientOp->addEntity.entityId));
    break;
  case SHOVELER_CLIENT_OP_REMOVE_ENTITY:
    g_string_append_len(
        output, (gchar*) &clientOp->removeEntity.entityId, sizeof(clientOp->removeEntity.entityId));
    break;
  case SHOVELER_CLIENT_OP_ADD_COMPONENT: {
    g_string_append_len(
        output, (gchar*) &clientOp->addComponent.entityId, sizeof(clientOp->addComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->addComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    break;
  }
  case SHOVELER_CLIENT_OP_UPDATE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &clientOp->updateComponent.entityId,
        sizeof(clientOp->updateComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->updateComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    g_string_append_len(
        output,
        (gchar*) &clientOp->updateComponent.fieldId,
        sizeof(clientOp->updateComponent.fieldId));
    if (!shovelerComponentFieldSerializeValue(clientOp->updateComponent.fieldValue, output)) {
      return false;
    }
    break;
  }
  case SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &clientOp->activateComponent.entityId,
        sizeof(clientOp->activateComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->activateComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    break;
  }
  case SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &clientOp->deactivateComponent.entityId,
        sizeof(clientOp->deactivateComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->deactivateComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    break;
  }
  case SHOVELER_CLIENT_OP_DELEGATE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &clientOp->delegateComponent.entityId,
        sizeof(clientOp->delegateComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->delegateComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    break;
  }
  case SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &clientOp->undelegateComponent.entityId,
        sizeof(clientOp->undelegateComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->undelegateComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    break;
  }
  case SHOVELER_CLIENT_OP_REMOVE_COMPONENT: {
    g_string_append_len(
        output,
        (gchar*) &clientOp->removeComponent.entityId,
        sizeof(clientOp->removeComponent.entityId));
    int componentTypeIndex = shovelerComponentTypeIndexerFromId(
        componentTypeIndexer, clientOp->removeComponent.componentTypeId);
    if (componentTypeIndex < 0) {
      return false;
    }
    g_string_append_len(output, (gchar*) &componentTypeIndex, sizeof(componentTypeIndex));
    break;
  }
  default:
    return false;
  }

  return true;
}

bool shovelerClientOpDeserialize(
    ShovelerClientOpWithData* clientOp,
    ShovelerComponentTypeIndexer* componentTypeIndexer,
    const unsigned char* buffer,
    int bufferSize,
    int* readIndex) {
  shovelerClientOpClearWithData(clientOp);

#define PARSE_VALUE(TARGET, TYPE) \
  if (*readIndex + sizeof(TYPE) > bufferSize) { \
    return false; \
  } \
  memcpy(&(TARGET), &buffer[*readIndex], sizeof(TYPE)); \
  (*readIndex) += sizeof(TYPE)

  char typeChar;
  PARSE_VALUE(typeChar, char);
  if (typeChar < 0 || typeChar > SHOVELER_CLIENT_OP_REMOVE_COMPONENT) {
    return false;
  }
  clientOp->op.type = (ShovelerClientOpType) typeChar;

  switch (clientOp->op.type) {
  case SHOVELER_CLIENT_OP_NOOP:
    break;
  case SHOVELER_CLIENT_OP_ADD_ENTITY:
    PARSE_VALUE(clientOp->op.addEntity.entityId, long long int);
    break;
  case SHOVELER_CLIENT_OP_REMOVE_ENTITY:
    PARSE_VALUE(clientOp->op.removeEntity.entityId, long long int);
    break;
  case SHOVELER_CLIENT_OP_ADD_COMPONENT: {
    PARSE_VALUE(clientOp->op.addComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.addComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.addComponent.componentTypeId == NULL) {
      return false;
    }
    break;
  }
  case SHOVELER_CLIENT_OP_UPDATE_COMPONENT: {
    PARSE_VALUE(clientOp->op.updateComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.updateComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.updateComponent.componentTypeId == NULL) {
      return false;
    }
    PARSE_VALUE(clientOp->op.updateComponent.fieldId, int);
    if (!shovelerComponentFieldDeserializeValue(
            &clientOp->fieldValue, buffer, bufferSize, readIndex)) {
      return false;
    }
    clientOp->op.updateComponent.fieldValue = &clientOp->fieldValue;
    break;
  }
  case SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT: {
    PARSE_VALUE(clientOp->op.activateComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.activateComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.activateComponent.componentTypeId == NULL) {
      return false;
    }
    break;
  }
  case SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT: {
    PARSE_VALUE(clientOp->op.deactivateComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.deactivateComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.deactivateComponent.componentTypeId == NULL) {
      return false;
    }
    break;
  }
  case SHOVELER_CLIENT_OP_DELEGATE_COMPONENT: {
    PARSE_VALUE(clientOp->op.delegateComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.delegateComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.delegateComponent.componentTypeId == NULL) {
      return false;
    }
    break;
  }
  case SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT: {
    PARSE_VALUE(clientOp->op.undelegateComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.undelegateComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.undelegateComponent.componentTypeId == NULL) {
      return false;
    }
    break;
  }
  case SHOVELER_CLIENT_OP_REMOVE_COMPONENT: {
    PARSE_VALUE(clientOp->op.removeComponent.entityId, long long int);
    int componentTypeIndex;
    PARSE_VALUE(componentTypeIndex, int);
    clientOp->op.removeComponent.componentTypeId =
        shovelerComponentTypeIndexerToId(componentTypeIndexer, componentTypeIndex);
    if (clientOp->op.removeComponent.componentTypeId == NULL) {
      return false;
    }
    break;
  }
  default:
    return false;
  }

#undef PARSE_VALUE

  return true;
}

char* shovelerClientOpDebugPrint(const ShovelerClientOp* clientOp) {
  GString* output = g_string_new("");

  switch (clientOp->type) {
  case SHOVELER_CLIENT_OP_NOOP:
    g_string_append_printf(output, "Noop()");
    break;
  case SHOVELER_CLIENT_OP_ADD_ENTITY:
    g_string_append_printf(output, "AddEntity(%lld)", clientOp->addEntity.entityId);
    break;
  case SHOVELER_CLIENT_OP_REMOVE_ENTITY:
    g_string_append_printf(output, "RemoveEntity(%lld)", clientOp->removeEntity.entityId);
    break;
  case SHOVELER_CLIENT_OP_ADD_COMPONENT:
    g_string_append_printf(
        output,
        "AddComponent(%lld, %s)",
        clientOp->addComponent.entityId,
        clientOp->addComponent.componentTypeId);
    break;
  case SHOVELER_CLIENT_OP_UPDATE_COMPONENT:
    g_string_append_printf(
        output,
        "UpdateComponent(%lld, %s:%d)",
        clientOp->updateComponent.entityId,
        clientOp->updateComponent.componentTypeId,
        clientOp->updateComponent.fieldId);
    break;
  case SHOVELER_CLIENT_OP_ACTIVATE_COMPONENT:
    g_string_append_printf(
        output,
        "ActivateComponent(%lld, %s)",
        clientOp->activateComponent.entityId,
        clientOp->activateComponent.componentTypeId);
    break;
  case SHOVELER_CLIENT_OP_DEACTIVATE_COMPONENT:
    g_string_append_printf(
        output,
        "DeactivateComponent(%lld, %s)",
        clientOp->deactivateComponent.entityId,
        clientOp->deactivateComponent.componentTypeId);
    break;
  case SHOVELER_CLIENT_OP_DELEGATE_COMPONENT:
    g_string_append_printf(
        output,
        "DelegateComponent(%lld, %s)",
        clientOp->delegateComponent.entityId,
        clientOp->delegateComponent.componentTypeId);
    break;
  case SHOVELER_CLIENT_OP_UNDELEGATE_COMPONENT:
    g_string_append_printf(
        output,
        "UndelegateComponent(%lld, %s)",
        clientOp->undelegateComponent.entityId,
        clientOp->undelegateComponent.componentTypeId);
    break;
  case SHOVELER_CLIENT_OP_REMOVE_COMPONENT: {
    g_string_append_printf(
        output,
        "RemoveComponent(%lld, %s)",
        clientOp->removeComponent.entityId,
        clientOp->removeComponent.componentTypeId);
    break;
  }
  default:
    g_string_append_printf(output, "(invalid client op))");
    break;
  }

  char* string = output->str;
  g_string_free(output, /* freeSegment */ false);
  return string;
}
