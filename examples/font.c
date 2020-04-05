#include <stdlib.h> // EXIT_SUCCESS

#include <shoveler/image/png.h>
#include <shoveler/font_atlas.h>
#include <shoveler/font.h>
#include <shoveler/image.h>
#include <shoveler/log.h>

int main(int argc, char *argv[])
{
	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);

	if(argc != 2) {
		shovelerLogError("Usage: %s [font file]", argv[0]);
	}

	const char *filename = argv[1];

	ShovelerFonts *fonts = shovelerFontsCreate();
	ShovelerFont *font = shovelerFontsLoadFontFile(fonts, "test font", filename);
	if(font == NULL) {
		shovelerFontsFree(fonts);
		return EXIT_FAILURE;
	}

	ShovelerFontAtlas *fontAtlas = shovelerFontAtlasCreate(font, 48, 1);

	for(char c = 'a'; c <= 'z'; c++) {
		shovelerFontAtlasGetGlyph(fontAtlas, c);
	}

	for(char c = 'A'; c <= 'Z'; c++) {
		shovelerFontAtlasGetGlyph(fontAtlas, c);
	}

	for(char c = '0'; c <= '9'; c++) {
		shovelerFontAtlasGetGlyph(fontAtlas, c);
	}

	shovelerFontAtlasValidateState(fontAtlas);
	shovelerImagePngWriteFile(fontAtlas->image, "atlas.png");

	shovelerFontAtlasFree(fontAtlas);
	shovelerFontsFree(fonts);

	return EXIT_SUCCESS;
}
