#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "shoveler/drawable/quad.h"
#include "shoveler/shader_program.h"
#include "shoveler/opengl.h"

typedef struct {
	char position[3];
	char normal[3];
	char uv[2];
} QuadVertex;

typedef struct {
	unsigned char indices[3];
} QuadTriangle;

typedef struct {
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
	GLuint indexBuffer;
} QuadData;

static bool drawQuad(ShovelerDrawable *quad);
static void freeQuad(ShovelerDrawable *quad);

static QuadVertex quadVertices[] = {
	{{-1, -1, 0}, {0, 0, 1}, {0, 0}},
	{{1, -1, 0}, {0, 0, 1}, {1, 0}},
	{{-1, 1, 0}, {0, 0, 1}, {0, 1}},
	{{1, 1, 0}, {0, 0, 1}, {1, 1}}};

static QuadTriangle quadTriangles[] = {
	{{0, 1, 2}},
	{{1, 3, 2}}};

ShovelerDrawable *shovelerDrawableQuadCreate()
{
	QuadData *quadData = malloc(sizeof(QuadData));
	ShovelerDrawable *quad = malloc(sizeof(ShovelerDrawable));
	quad->draw = drawQuad;
	quad->free = freeQuad;
	quad->data = quadData;

	glGenVertexArrays(1, &quadData->vertexArrayObject);
	glBindVertexArray(quadData->vertexArrayObject);
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION);
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL);
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV);
	glVertexAttribFormat(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 3, GL_BYTE, GL_FALSE, offsetof(QuadVertex, position));
	glVertexAttribFormat(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL, 3, GL_BYTE, GL_FALSE, offsetof(QuadVertex, normal));
	glVertexAttribFormat(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, 2, GL_BYTE, GL_FALSE, offsetof(QuadVertex, uv));
	glVertexAttribBinding(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 0);
	glVertexAttribBinding(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL, 0);
	glVertexAttribBinding(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, 0);

	glGenBuffers(1, &quadData->vertexBuffer);
	glGenBuffers(1, &quadData->indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, quadData->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QuadVertex), quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadData->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(QuadTriangle), quadTriangles, GL_STATIC_DRAW);

	if(!shovelerOpenGLCheckSuccess()) {
		freeQuad(quad);
		return NULL;
	}

	return quad;
}

static bool drawQuad(ShovelerDrawable *quad)
{
	QuadData *quadData = quad->data;

	glBindVertexArray(quadData->vertexArrayObject);
	glBindVertexBuffer(0, quadData->vertexBuffer, 0, sizeof(QuadVertex));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadData->indexBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);

	return shovelerOpenGLCheckSuccess();
}

static void freeQuad(ShovelerDrawable *quad)
{
	QuadData *quadData = quad->data;
	glDeleteVertexArrays(1, &quadData->vertexArrayObject);
	glDeleteBuffers(1, &quadData->vertexBuffer);
	glDeleteBuffers(1, &quadData->indexBuffer);

	free(quadData);
	free(quad);
}
