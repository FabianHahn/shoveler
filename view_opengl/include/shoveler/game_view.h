#ifndef SHOVELER_GAME_VIEW_H
#define SHOVELER_GAME_VIEW_H

#include <shoveler/game.h>
#include <shoveler/view.h>

ShovelerView *shovelerGameViewCreate(ShovelerGame *game, ShovelerViewUpdateAuthoritativeComponentFunction *updateAuthoritativeComponent, void *updateAuthoritativeComponentUserData);

#endif
