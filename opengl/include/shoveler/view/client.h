#ifndef SHOVELER_VIEW_CLIENT_H
#define SHOVELER_VIEW_CLIENT_H

#include <shoveler/view.h>

static const char *shovelerViewClientComponentName = "client";

bool shovelerViewAddEntityClient(ShovelerView *view, long long int entityId);
bool shovelerViewDelegateClient(ShovelerView *view, long long int entityId);
bool shovelerViewUndelegateClient(ShovelerView *view, long long int entityId);
bool shovelerViewRemoveEntityClient(ShovelerView *view, long long int entityId);

#endif
