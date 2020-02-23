#ifndef SHOVELER_VIEW_IMAGE_H
#define SHOVELER_VIEW_IMAGE_H

#include <stdbool.h> // bool
#include <stddef.h> // size_t

#include <shoveler/component/image.h>
#include <shoveler/view.h>

typedef struct {
	ShovelerComponentImageFormat format;
	long long int resourceEntityId;
} ShovelerViewImageConfiguration;

ShovelerComponent *shovelerViewEntityAddImage(ShovelerViewEntity *entity, const ShovelerViewImageConfiguration *configuration);
ShovelerImage *shovelerViewEntityGetImage(ShovelerViewEntity *entity);
bool shovelerViewEntityGetImageConfiguration(ShovelerViewEntity *entity, ShovelerViewImageConfiguration *outputConfiguration);
bool shovelerViewEntityUpdateImageConfiguration(ShovelerViewEntity *entity, const ShovelerViewImageConfiguration *configuration);
bool shovelerViewEntityRemoveImage(ShovelerViewEntity *entity);

static inline ShovelerComponent *shovelerViewEntityGetImageComponent(ShovelerViewEntity *entity)
{
	return shovelerViewEntityGetComponent(entity, shovelerComponentTypeIdImage);
}

#endif
