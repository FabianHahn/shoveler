#ifndef SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_H
#define SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_H

typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h
typedef struct ShovelerTextTextureRendererStruct ShovelerTextTextureRenderer; // forward declaration: text_texture_renderer.h

extern const char *const shovelerComponentTypeIdTextTextureRenderer;

typedef enum {
	SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_OPTION_ID_FONT_ATLAS_TEXTURE,
} ShovelerComponentTextTextureRendererOptionId;

ShovelerComponentType *shovelerComponentCreateTextTextureRendererType();
ShovelerTextTextureRenderer *shovelerComponentGetTextTextureRenderer(ShovelerComponent *component);

#endif
