#include "../drawable/point.h"

#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "../shader_program.h"
#include "opengl.h"

typedef struct {
	char position[3];
} PointVertex;

typedef struct {
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
} PointData;

static bool drawPoint(ShovelerDrawable *point);
static void freePoint(ShovelerDrawable *point);

static PointVertex pointVertices[] = {{{0, 0, 0}}};

ShovelerDrawable *shovelerDrawablePointCreate()
{
	PointData *pointData = malloc(sizeof(PointData));
	ShovelerDrawable *point = malloc(sizeof(ShovelerDrawable));
	point->draw = drawPoint;
	point->free = freePoint;
	point->data = pointData;

	glGenVertexArrays(1, &pointData->vertexArrayObject);
	glBindVertexArray(pointData->vertexArrayObject);
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION);
	glVertexAttribFormat(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 3, GL_BYTE, GL_FALSE, offsetof(PointVertex, position));
	glVertexAttribBinding(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 0);

	glGenBuffers(1, &pointData->vertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, pointData->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PointVertex), pointVertices, GL_STATIC_DRAW);

	if(!shovelerOpenGLCheckSuccess()) {
		freePoint(point);
		return NULL;
	}

	return point;
}

static bool drawPoint(ShovelerDrawable *point)
{
	PointData *pointData = point->data;

	glBindVertexArray(pointData->vertexArrayObject);
	glBindVertexBuffer(0, pointData->vertexBuffer, 0, sizeof(PointVertex));
	glDrawArrays(GL_POINTS, 0, 1);

	return shovelerOpenGLCheckSuccess();
}

static void freePoint(ShovelerDrawable *point)
{
	PointData *pointData = point->data;
	glDeleteVertexArrays(1, &pointData->vertexArrayObject);
	glDeleteBuffers(1, &pointData->vertexBuffer);

	free(point);
}
