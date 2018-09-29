#ifndef SHOVELER_VIEW_CLIENT_H
#define SHOVELER_VIEW_CLIENT_H

#include <shoveler/view.h>

static const char *shovelerViewClientComponentName = "client";

bool shovelerViewEntityAddClient(ShovelerViewEntity *entity);
bool shovelerViewEntityDelegateClient(ShovelerViewEntity *entity);
bool shovelerViewEntityUndelegateClient(ShovelerViewEntity *entity);
bool shovelerViewEntityRemoveClient(ShovelerViewEntity *entity);

#endif
