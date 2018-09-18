#include <stdlib.h> // malloc free

#include "shoveler/drawable/cube.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/drawable/point.h"
#include "shoveler/view/drawables.h"

ShovelerViewDrawables *shovelerViewDrawablesCreate(ShovelerView *view)
{
	ShovelerViewDrawables *drawables = malloc(sizeof(ShovelerViewDrawables));
	drawables->entities = g_hash_table_new(g_int64_hash, g_int64_equal);
	drawables->cube = shovelerDrawableCubeCreate();
	drawables->quad = shovelerDrawableQuadCreate();
	drawables->point = shovelerDrawablePointCreate();

	shovelerViewSetTarget(view, shovelerViewDrawablesTargetName, drawables);

	return drawables;
}

void shovelerViewDrawablesFree(ShovelerViewDrawables *drawables)
{
	shovelerDrawableFree(drawables->cube);
	shovelerDrawableFree(drawables->quad);
	shovelerDrawableFree(drawables->point);
	g_hash_table_destroy(drawables->entities);
	free(drawables);
}
