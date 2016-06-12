#ifndef SHOVELER_MODEL_H
#define SHOVELER_MODEL_H

#include <stdbool.h> // bool

#include <GL/glew.h>

#include "drawable.h"
#include "material.h"
#include "types.h"
#include "uniform_map.h"

typedef struct {
	ShovelerDrawable *drawable;
	ShovelerMaterial *material;
	ShovelerVector3 translation;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	ShovelerMatrix transformation;
	ShovelerMatrix normalTransformation;
	bool visible;
	GLuint polygonMode;
	ShovelerUniformMap *uniforms;
} ShovelerModel;

ShovelerModel *shovelerModelCreate(ShovelerDrawable *drawable, ShovelerMaterial *material);
bool shovelerModelRender(ShovelerModel *model);
void shovelerModelFree(ShovelerModel *model);

#endif
