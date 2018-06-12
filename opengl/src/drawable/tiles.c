#include <stdbool.h> // bool
#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "shoveler/drawable/tiles.h"
#include "shoveler/shader_program.h"
#include "shoveler/opengl.h"

typedef struct {
	unsigned char position[2];
	unsigned char uv[2];
} TilesVertex;

typedef struct {
	unsigned int indices[3];
} TilesTriangle;

typedef struct {
	ShovelerDrawable drawable;
	unsigned char width;
	unsigned char height;
	TilesVertex *vertices;
	TilesTriangle *triangles;
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
	GLuint indexBuffer;
} Tiles;

static bool drawTiles(ShovelerDrawable *tilesDrawable);
static void freeTiles(ShovelerDrawable *tilesDrawable);

ShovelerDrawable *shovelerDrawableTilesCreate(unsigned char width, unsigned char height)
{
	Tiles *tiles = malloc(sizeof(Tiles));
	tiles->width = width;
	tiles->height = height;
	tiles->vertices = malloc(4 * width * height * sizeof(TilesVertex));
	tiles->triangles = malloc(2 * width * height * sizeof(TilesTriangle));
	tiles->drawable.data = tiles;
	tiles->drawable.draw = drawTiles;
	tiles->drawable.free = freeTiles;

	for(unsigned char x = 0; x < width; x++) {
		for(unsigned char y = 0; y < height; y++) {
			unsigned int index = x * height + y;

			tiles->vertices[4 * index + 0].position[0] = x;
			tiles->vertices[4 * index + 0].position[1] = y;
			tiles->vertices[4 * index + 0].uv[0] = 0;
			tiles->vertices[4 * index + 0].uv[1] = 0;

			tiles->vertices[4 * index + 1].position[0] = x + 1;
			tiles->vertices[4 * index + 1].position[1] = y;
			tiles->vertices[4 * index + 1].uv[0] = 1;
			tiles->vertices[4 * index + 1].uv[1] = 0;

			tiles->vertices[4 * index + 2].position[0] = x;
			tiles->vertices[4 * index + 2].position[1] = y + 1;
			tiles->vertices[4 * index + 2].uv[0] = 0;
			tiles->vertices[4 * index + 2].uv[1] = 1;

			tiles->vertices[4 * index + 3].position[0] = x + 1;
			tiles->vertices[4 * index + 3].position[1] = y + 1;
			tiles->vertices[4 * index + 3].uv[0] = 1;
			tiles->vertices[4 * index + 3].uv[1] = 1;

			tiles->triangles[2 * index + 0].indices[0] = 4 * index + 0;
			tiles->triangles[2 * index + 0].indices[1] = 4 * index + 1;
			tiles->triangles[2 * index + 0].indices[2] = 4 * index + 2;

			tiles->triangles[2 * index + 1].indices[0] = 4 * index + 1;
			tiles->triangles[2 * index + 1].indices[1] = 4 * index + 3;
			tiles->triangles[2 * index + 1].indices[2] = 4 * index + 2;
		}
	}

	glGenVertexArrays(1, &tiles->vertexArrayObject);
	glBindVertexArray(tiles->vertexArrayObject);
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION);
	glEnableVertexAttribArray(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV);
	glVertexAttribFormat(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 2, GL_UNSIGNED_BYTE, GL_FALSE, offsetof(TilesVertex, position));
	glVertexAttribFormat(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, 2, GL_UNSIGNED_BYTE, GL_FALSE, offsetof(TilesVertex, uv));
	glVertexAttribBinding(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_POSITION, 0);
	glVertexAttribBinding(SHOVELER_SHADER_PROGRAM_ATTRIBUTE_UV, 0);

	glGenBuffers(1, &tiles->vertexBuffer);
	glGenBuffers(1, &tiles->indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, tiles->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * width * height * sizeof(TilesVertex), tiles->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tiles->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * width * height * sizeof(TilesTriangle), tiles->triangles, GL_STATIC_DRAW);

	if(!shovelerOpenGLCheckSuccess()) {
		freeTiles(&tiles->drawable);
		return NULL;
	}

	return &tiles->drawable;
}

static bool drawTiles(ShovelerDrawable *tilesDrawable)
{
	Tiles *tiles = tilesDrawable->data;

	glBindVertexArray(tiles->vertexArrayObject);
	glBindVertexBuffer(0, tiles->vertexBuffer, 0, sizeof(TilesVertex));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tiles->indexBuffer);
	glDrawElements(GL_TRIANGLES, 2 * 3 * tiles->width * tiles->height, GL_UNSIGNED_INT, NULL);

	return shovelerOpenGLCheckSuccess();
}

static void freeTiles(ShovelerDrawable *tilesDrawable)
{
	Tiles *tiles = tilesDrawable->data;
	glDeleteVertexArrays(1, &tiles->vertexArrayObject);
	glDeleteBuffers(1, &tiles->vertexBuffer);
	glDeleteBuffers(1, &tiles->indexBuffer);

	free(tiles->triangles);
	free(tiles->vertices);
	free(tiles);
}
