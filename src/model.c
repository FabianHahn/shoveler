#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include "log.h"
#include "model.h"
#include "opengl.h"
#include "types.h"
#include "uniform.h"

ShovelerModel *shovelerModelCreate(ShovelerDrawable *drawable, ShovelerMaterial *material)
{
	ShovelerModel *model = malloc(sizeof(ShovelerModel));
	model->drawable = drawable;
	model->material = material;
	model->translation = (ShovelerVector3){{0, 0, 0}};
	model->rotation = (ShovelerVector3){{0, 0, 0}};
	model->scale = (ShovelerVector3){{1, 1, 1}};
	model->transformation = shovelerMatrixIdentity;
	model->normalTransformation = shovelerMatrixIdentity;
	model->visible = true;
	model->polygonMode = GL_FILL;
	model->uniforms = shovelerUniformMapCreate();

	ShovelerUniform *modelUniform = shovelerUniformCreateMatrixPointer(&model->transformation);
	shovelerUniformMapInsert(model->uniforms, "model", modelUniform);
	ShovelerUniform *modelNormalUniform = shovelerUniformCreateMatrixPointer(&model->normalTransformation);
	shovelerUniformMapInsert(model->uniforms, "modelNormal", modelNormalUniform);

	return model;
}

bool shovelerModelRender(ShovelerModel *model)
{
	glPolygonMode(GL_FRONT_AND_BACK, model->polygonMode);

	if(!shovelerDrawableDraw(model->drawable)) {
		shovelerLogError("Failed to draw drawable when trying to render model");
		return false;
	}

	return shovelerOpenGLCheckSuccess();
}

void shovelerModelFree(ShovelerModel *model)
{
	shovelerUniformMapFree(model->uniforms);
	free(model);
}
