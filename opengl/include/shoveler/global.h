#ifndef SHOVELER_GLOBAL_H
#define SHOVELER_GLOBAL_H

#include <glib.h>
#include <shoveler/log.h>
#include <stdbool.h>
#include <stdio.h> // FILE

typedef struct {
  GHashTable* games;
} ShovelerGlobalContext;

bool shovelerGlobalInit();
ShovelerGlobalContext* shovelerGlobalGetContext();
void shovelerGlobalUninit();

#endif