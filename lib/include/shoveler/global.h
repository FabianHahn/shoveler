#ifndef SHOVELER_GLOBAL_H
#define SHOVELER_GLOBAL_H

#include <stdbool.h>
#include <stdio.h> // FILE

#include <glib.h>

#include <shoveler/log.h>

typedef struct {
	GHashTable *games;
} ShovelerGlobalContext;

bool shovelerGlobalInit();
ShovelerGlobalContext *shovelerGlobalGetContext();
void shovelerGlobalUninit();

#endif