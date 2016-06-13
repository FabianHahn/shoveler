#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include <GL/glew.h>

#include "drawables/cube.h"

#include "../shader_program.h"
#include "opengl.h"

typedef struct {
	char position[3];
	char normal[3];
	char uv[2];
} CubeVertex;

typedef struct {
	unsigned char indices[3];
} CubeTriangle;

typedef struct {
	GLuint vertexBuffer;
	GLuint indexBuffer;
} CubeData;

static bool drawCube(ShovelerDrawable *cube);
static void freeCube(ShovelerDrawable *cube);

static CubeVertex cubeVertices[] = {
	{{1, -1, -1}, {0, 0, -1}, {0, 0}},
	{{-1, -1, -1}, {0, 0, -1}, {1, 0}},
	{{-1, 1, -1}, {0, 0, -1}, {1, 1}},
	{{1, 1, -1}, {0, 0, -1}, {0, 1}},

	{{-1, 1, 1}, {0, 1, 0}, {0, 0}},
	{{1, 1, 1}, {0, 1, 0}, {1, 0}},
	{{1, 1, -1}, {0, 1, 0}, {1, 1}},
	{{-1, 1, -1}, {0, 1, 0}, {0, 1}},

	{{-1, -1, -1}, {-1, 0, 0}, {0, 0}},
	{{-1, -1, 1}, {-1, 0, 0}, {1, 0}},
	{{-1, 1, 1}, {-1, 0, 0}, {1, 1}},
	{{-1, 1, -1}, {-1, 0, 0}, {0, 1}},

	{{1, -1, 1}, {1, 0, 0}, {0, 0}},
	{{1, -1, -1}, {1, 0, 0}, {1, 0}},
	{{1, 1, -1}, {1, 0, 0}, {1, 1}},
	{{1, 1, 1}, {1, 0, 0}, {0, 1}},

	{{-1, -1, -1}, {0, -1, 0}, {0, 0}},
	{{1, -1, -1}, {0, -1, 0}, {1, 0}},
	{{1, -1, 1}, {0, -1, 0}, {1, 1}},
	{{-1, -1, 1}, {0, -1, 0}, {0, 1}},

	{{-1, -1, 1}, {0, 0, 1}, {0, 0}},
	{{1, -1, 1}, {0, 0, 1}, {1, 0}},
	{{1, 1, 1}, {0, 0, 1}, {1, 1}},
	{{-1, 1, 1}, {0, 0, 1}, {0, 1}}};

static CubeTriangle cubeTriangles[] = {
	{0, 1, 2},
	{0, 2, 3},

	{4, 5, 6},
	{4, 6, 7},

	{8, 9, 10},
	{8, 10, 11},

	{12, 13, 14},
	{12, 14, 15},

	{16, 17, 18},
	{16, 18, 19},

	{20, 21, 22},
	{20, 22, 23}};

ShovelerDrawable *shovelerDrawablesCubeCreate()
{
	CubeData *cubeData = malloc(sizeof(CubeData));
	ShovelerDrawable *cube = malloc(sizeof(ShovelerDrawable));
	cube->draw = drawCube;
	cube->free = freeCube;
	cube->data = cubeData;

	// fixme
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &cubeData->vertexBuffer);
	glGenBuffers(1, &cubeData->indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, cubeData->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(CubeVertex), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeData->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(CubeTriangle), cubeTriangles, GL_STATIC_DRAW);

	if(!shovelerOpenGLCheckSuccess()) {
		freeCube(cube);
		return NULL;
	}

	return cube;
}

static bool drawCube(ShovelerDrawable *cube)
{
	CubeData *cubeData = cube->data;

	glBindBuffer(GL_ARRAY_BUFFER, cubeData->vertexBuffer);

	glVertexAttribPointer(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 3, GL_BYTE, GL_FALSE, sizeof(CubeVertex), NULL + offsetof(CubeVertex, position));
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION);
	glVertexAttribPointer(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL, 3, GL_BYTE, GL_FALSE, sizeof(CubeVertex), NULL + offsetof(CubeVertex, normal));
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_NORMAL);
	glVertexAttribPointer(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, 2, GL_BYTE, GL_FALSE, sizeof(CubeVertex), NULL + offsetof(CubeVertex, uv));
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV);

	if(!shovelerOpenGLCheckSuccess()) {
		return false;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeData->indexBuffer);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);

	return shovelerOpenGLCheckSuccess();
}

static void freeCube(ShovelerDrawable *cube)
{
	CubeData *cubeData = cube->data;
	glDeleteBuffers(1, &cubeData->vertexBuffer);
	glDeleteBuffers(1, &cubeData->indexBuffer);

	free(cubeData);
	free(cube);
}
