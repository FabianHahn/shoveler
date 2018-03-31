#ifndef SHOVELER_MODEL_H
#define SHOVELER_MODEL_H

#include <stdbool.h> // bool

#include <glad/glad.h>

#include <shoveler/drawable.h>
#include <shoveler/material.h>
#include <shoveler/types.h>
#include <shoveler/uniform_map.h>

typedef struct {
	ShovelerDrawable *drawable;
	ShovelerMaterial *material;
	ShovelerVector3 translation;
	ShovelerVector3 rotation;
	ShovelerVector3 scale;
	ShovelerMatrix transformation;
	ShovelerMatrix normalTransformation;
	bool visible;
	bool emitter;
	bool screenspace;
	bool castsShadow;
	GLuint polygonMode;
	ShovelerUniformMap *uniforms;
} ShovelerModel;

ShovelerModel *shovelerModelCreate(ShovelerDrawable *drawable, ShovelerMaterial *material);
void shovelerModelUpdateTransformation(ShovelerModel *model);
bool shovelerModelRender(ShovelerModel *model);
void shovelerModelFree(ShovelerModel *model);

#endif
