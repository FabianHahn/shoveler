#ifndef SHOVELER_COLOR_H
#define SHOVELER_COLOR_H

#include <shoveler/types.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} ShovelerColor;

/** Converts a HSV color where each argument is in the [0,1] range to an RGB color. */
ShovelerColor shovelerColorFromHsv(float h, float s, float v);

static inline ShovelerVector3 shovelerColorToVector3(ShovelerColor color)
{
	return shovelerVector3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
}

static inline ShovelerColor shovelerColor(unsigned char r, unsigned char g, unsigned char b)
{
	ShovelerColor color;
	color.r = r;
	color.g = g;
	color.b = b;
	return color;
}

#endif
