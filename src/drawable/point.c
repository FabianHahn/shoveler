#include "../drawable/point.h"

#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "../shader_program.h"
#include "opengl.h"

static bool drawPoint(ShovelerDrawable *point);
static void freePoint(ShovelerDrawable *point);

ShovelerDrawable *shovelerDrawablePointCreate()
{
	ShovelerDrawable *point = malloc(sizeof(ShovelerDrawable));
	point->draw = drawPoint;
	point->free = freePoint;
	point->data = NULL;
	return point;
}

static bool drawPoint(ShovelerDrawable *point)
{
	glDrawArrays(GL_POINTS, 0, 1);
	return shovelerOpenGLCheckSuccess();
}

static void freePoint(ShovelerDrawable *point)
{
	free(point);
}
