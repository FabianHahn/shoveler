#include <math.h> // fabsf fmodf roundf

#include "shoveler/color.h"

ShovelerColor shovelerColorFromHsv(float h, float s, float v)
{
	// based on: https://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB
	float c = s * v;
	float hp = h * 360.0f / 60.0f;
	float x = c * (1.0f - fabsf(fmodf(hp, 2.0f) - 1.0f));
	float m = v - c;

	float r1 = m;
	float g1 = m;
	float b1 = m;
	if(hp < 0.0f) {
		// just leave it
	} else if(hp < 1.0f) {
		r1 += c;
		g1 += x;
	} else if(hp < 2.0f) {
		r1 += x;
		g1 += c;
	} else if(hp < 3.0f) {
		g1 += c;
		b1 += x;
	} else if(hp < 4.0f) {
		g1 += x;
		b1 += c;
	} else if(hp < 5.0f) {
		r1 += x;
		b1 += c;
	} else if(hp <= 6.0f) {
		r1 += c;
		b1 += x;
	}

	unsigned char r2 = (unsigned char) roundf(255 * r1);
	unsigned char g2 = (unsigned char) roundf(255 * g1);
	unsigned char b2 = (unsigned char) roundf(255 * b1);
	return (ShovelerColor){r2, g2, b2};
}
