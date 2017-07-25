#ifndef SHOVELER_GLOBAL_H
#define SHOVELER_GLOBAL_H

#include <stdbool.h>
#include <stdio.h> // FILE

#include <glib.h>

#include "log.h"

typedef struct {
	GHashTable *games;
} ShovelerGlobalContext;

bool shovelerGlobalInit(ShovelerLogLevel logLevel, FILE *logChannel);
ShovelerGlobalContext *shovelerGlobalGetContext();
void shovelerGlobalUninit();

#endif