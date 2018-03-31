#include <cstdlib> // free

#include "fakeglib/GMemory.h"

FAKEGLIB_API void g_free(gpointer mem)
{
	free(mem);
}
