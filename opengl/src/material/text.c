#include "shoveler/material/text.h"

#include <stdlib.h> // malloc free

#include "shoveler/camera.h"
#include "shoveler/font_atlas.h"
#include "shoveler/font_atlas_texture.h"
#include "shoveler/light.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/model.h"
#include "shoveler/sampler.h"
#include "shoveler/scene.h"
#include "shoveler/shader.h"
#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/shader_program/model_vertex.h"

static const char* fragmentShaderSource =
    "#version 400\n"
    "\n"
    "uniform bool sceneDebugMode;\n"
    "uniform vec2 regionPosition;\n"
    "uniform vec2 regionSize;\n"
    "uniform sampler2D fontAtlas;\n"
    "uniform uint fontSize;\n"
    "uniform vec2 textCorner;\n"
    "uniform float textSize;\n"
    "uniform vec4 textColor;\n"
    "uniform float characterAdvance;\n"
    "uniform uint glyphMinX;\n"
    "uniform uint glyphMinY;\n"
    "uniform uint glyphWidth;\n"
    "uniform uint glyphHeight;\n"
    "uniform uint glyphBearingX;\n"
    "uniform uint glyphBearingY;\n"
    "uniform bool glyphIsRotated;\n"
    "\n"
    "flat in vec2 fragmentPosition;\n"
    "in vec2 fragmentUv;\n"
    ""
    "in vec3 worldPosition;\n"
    "in vec3 worldNormal;\n"
    "in vec2 worldUv;\n"
    "in vec4 lightFrustumPosition4;\n"
    "\n"
    "out vec4 fragmentColor;\n"
    "\n"
    "vec2 getSpriteUv(vec2 spriteCorner, vec2 spriteSize) {\n"
    "	// Compute the UV coordinate offset of this fragment from the sprite corner.\n"
    "	vec2 regionCorner = regionPosition - 0.5 * regionSize;\n"
    "	vec2 spriteOffset = spriteCorner - regionCorner;\n"
    "	vec2 spriteCornerRegionUv = spriteOffset / regionSize;\n"
    "	vec2 spriteWorldUvOffset = worldUv - spriteCornerRegionUv;\n"
    ""
    "	// Compute the UV coordinates of the sprite of this fragment within a hypothetical sprite "
    "texture.\n"
    "	vec2 spriteUvScale = regionSize / spriteSize;\n"
    "	vec2 spriteUv = spriteWorldUvOffset * spriteUvScale;\n"
    "	return spriteUv;\n"
    "}\n"
    "\n"
    "vec2 getFontAtlasUv(vec2 glyphUv) {\n"
    "	vec2 fontAtlasGlyphCorner = vec2(glyphMinX, glyphMinY);\n"
    ""
    "	vec2 glyphRotatedUv;\n"
    "	vec2 fontAtlasGlyphSize;\n"
    "	if(glyphIsRotated) {;\n"
    "		glyphRotatedUv = vec2(1.0 - glyphUv.y, glyphUv.x);\n"
    "		fontAtlasGlyphSize = vec2(glyphHeight, glyphWidth);\n"
    "	} else {\n"
    "		glyphRotatedUv = glyphUv;\n"
    "		fontAtlasGlyphSize = vec2(glyphWidth, glyphHeight);\n"
    "	}\n"
    ""
    "	vec2 fontAtlasSize = textureSize(fontAtlas, 0);\n"
    "	vec2 fontAtlasInverseSize = 1.0 / fontAtlasSize;\n"
    "	vec2 fontAtlasGlyphUvScaleFactor = fontAtlasGlyphSize * fontAtlasInverseSize;\n"
    "	vec2 fontAtlasGlyphCornerUv = fontAtlasGlyphCorner * fontAtlasInverseSize;\n"
    "	vec2 fontAtlasUv = fontAtlasGlyphCornerUv + fontAtlasGlyphUvScaleFactor * glyphRotatedUv;\n"
    "	return fontAtlasUv;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "	// Translate glyph metrics into the region's coordinate system.\n"
    "	float textFactor = textSize / fontSize;\n"
    "	float characterGlyphAdvance = textFactor * characterAdvance;\n"
    "	vec2 characterGlyphSize = textFactor * vec2(glyphWidth, glyphHeight);\n"
    "	vec2 characterGlyphBearing = textFactor * vec2(glyphBearingX, glyphBearingY);\n"
    ""
    "	// Compute the corner of the character glyph area to render within the region.\n"
    "	vec2 characterCorner = textCorner + vec2(characterGlyphAdvance, 0.0);\n"
    "	vec2 characterGlyphCorner = characterCorner + characterGlyphBearing - vec2(0.0, "
    "characterGlyphSize.y);\n"
    ""
    "	vec2 glyphUv = getSpriteUv(characterGlyphCorner, characterGlyphSize);\n"
    ""
    "	if (glyphUv.x >= 0.0 &&\n"
    "		glyphUv.x <= 1.0 &&\n"
    "		glyphUv.y >= 0.0 &&\n"
    "		glyphUv.y <= 1.0) {\n"
    "		vec2 fontAtlasUv = getFontAtlasUv(glyphUv);\n"
    "		float fontAtlasValue = texture2D(fontAtlas, fontAtlasUv).r;\n"
    ""
    "		if (sceneDebugMode) {\n"
    "			fragmentColor = vec4(fontAtlasUv.xy, fontAtlasUv.y, 1.0);\n"
    "		} else {\n"
    "			fragmentColor = vec4(textColor.rgb, fontAtlasValue * textColor.a);\n"
    "		}\n"
    "	} else {\n"
    "		fragmentColor = vec4(0.0);\n"
    "	}\n"
    "}\n";

typedef struct {
  ShovelerMaterial* material;
  ShovelerSampler* sampler;
  ShovelerVector2 activeRegionPosition;
  ShovelerVector2 activeRegionSize;
  ShovelerFontAtlasTexture* activeFontAtlasTexture;
  ShovelerTexture* activeTexture;
  unsigned int activeFontSize;
  const char* activeText;
  ShovelerVector2 activeTextCorner;
  float activeTextSize;
  ShovelerVector4 activeTextColor;
  float activeCharacterAdvance;
  unsigned int activeGlyphMinX;
  unsigned int activeGlyphMinY;
  unsigned int activeGlyphWidth;
  unsigned int activeGlyphHeight;
  unsigned int activeGlyphBearingX;
  unsigned int activeGlyphBearingY;
  bool activeGlyphIsRotated;
} MaterialData;

static bool render(
    ShovelerMaterial* material,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerRenderState* renderState);
static void freeMaterialData(ShovelerMaterial* material);

ShovelerMaterial* shovelerMaterialTextCreate(ShovelerShaderCache* shaderCache, bool screenspace) {
  GLuint vertexShaderObject = shovelerShaderProgramModelVertexCreate(screenspace);
  GLuint fragmentShaderObject =
      shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

  MaterialData* materialData = malloc(sizeof(MaterialData));
  materialData->material = shovelerMaterialCreate(shaderCache, screenspace, program);
  materialData->material->data = materialData;
  materialData->material->render = render;
  materialData->material->freeData = freeMaterialData;
  materialData->sampler = shovelerSamplerCreate(true, false, false);
  materialData->activeFontAtlasTexture = NULL;
  materialData->activeTexture = NULL;
  materialData->activeFontSize = 0;
  materialData->activeTextCorner = shovelerVector2(0.0f, 0.0f);
  materialData->activeTextSize = 0.0f;
  materialData->activeTextColor = shovelerVector4(1.0f, 1.0f, 1.0f, 1.0f);
  materialData->activeCharacterAdvance = 0.0f;
  materialData->activeGlyphMinX = 0;
  materialData->activeGlyphMinY = 0;
  materialData->activeGlyphWidth = 0;
  materialData->activeGlyphHeight = 0;
  materialData->activeGlyphBearingX = 0;
  materialData->activeGlyphBearingY = 0;
  materialData->activeGlyphIsRotated = false;

  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "regionPosition",
      shovelerUniformCreateVector2Pointer(&materialData->activeRegionPosition));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "regionSize",
      shovelerUniformCreateVector2Pointer(&materialData->activeRegionSize));

  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "fontAtlas",
      shovelerUniformCreateTexturePointer(&materialData->activeTexture, &materialData->sampler));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "fontSize",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeFontSize));

  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "textCorner",
      shovelerUniformCreateVector2Pointer(&materialData->activeTextCorner));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "textSize",
      shovelerUniformCreateFloatPointer(&materialData->activeTextSize));

  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "textColor",
      shovelerUniformCreateVector4Pointer(&materialData->activeTextColor));

  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "characterAdvance",
      shovelerUniformCreateFloatPointer(&materialData->activeCharacterAdvance));

  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphMinX",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeGlyphMinX));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphMinY",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeGlyphMinY));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphWidth",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeGlyphWidth));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphHeight",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeGlyphHeight));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphBearingX",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeGlyphBearingX));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphBearingY",
      shovelerUniformCreateUnsignedIntPointer(&materialData->activeGlyphBearingY));
  shovelerUniformMapInsert(
      materialData->material->uniforms,
      "glyphIsRotated",
      shovelerUniformCreateBoolPointer(&materialData->activeGlyphIsRotated));

  return materialData->material;
}

void shovelerMaterialTextSetActiveRegion(
    ShovelerMaterial* material, ShovelerVector2 regionPosition, ShovelerVector2 regionSize) {
  MaterialData* materialData = material->data;
  materialData->activeRegionPosition = regionPosition;
  materialData->activeRegionSize = regionSize;
}

void shovelerMaterialTextSetActiveFontAtlasTexture(
    ShovelerMaterial* material, ShovelerFontAtlasTexture* fontAtlasTexture) {
  MaterialData* materialData = material->data;
  materialData->activeFontAtlasTexture = fontAtlasTexture;
  materialData->activeFontSize = fontAtlasTexture->fontAtlas->fontSize;
}

void shovelerMaterialTextSetActiveText(
    ShovelerMaterial* material, const char* text, ShovelerVector2 corner, float size) {
  MaterialData* materialData = material->data;
  materialData->activeText = text;
  materialData->activeTextCorner = corner;
  materialData->activeTextSize = size;
}

void shovelerMaterialTextSetActiveColor(ShovelerMaterial* material, ShovelerVector4 color) {
  MaterialData* materialData = material->data;
  materialData->activeTextColor = color;
}

static bool render(
    ShovelerMaterial* material,
    ShovelerScene* scene,
    ShovelerCamera* camera,
    ShovelerLight* light,
    ShovelerModel* model,
    ShovelerRenderState* renderState) {
  MaterialData* materialData = material->data;

  // since we are only changing uniform pointer values per per glyph, we can reuse the same shader
  // for all of them
  ShovelerShader* shader = shovelerSceneGenerateShader(scene, camera, light, model, material, NULL);

  shovelerRenderStateEnableBlend(renderState, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  materialData->activeTexture = materialData->activeFontAtlasTexture->texture;
  materialData->activeCharacterAdvance = 0.0f;

  for (const char* c = materialData->activeText; *c != '\0'; c++) {
    unsigned char character = *((unsigned char*) c);

    ShovelerFontAtlasGlyph* glyph =
        shovelerFontAtlasGetGlyph(materialData->activeFontAtlasTexture->fontAtlas, character);
    materialData->activeGlyphMinX = glyph->minX;
    materialData->activeGlyphMinY = glyph->minY;
    materialData->activeGlyphWidth = glyph->width;
    materialData->activeGlyphHeight = glyph->height;
    materialData->activeGlyphBearingX = glyph->bearingX;
    materialData->activeGlyphBearingY = glyph->bearingY;
    materialData->activeGlyphIsRotated = glyph->isRotated;

    if (!shovelerShaderUse(shader)) {
      shovelerLogWarning(
          "Failed to use shader for character %c when rendering text '%s' with material %p and "
          "model %p.",
          character,
          materialData->activeText,
          material,
          model);
      return false;
    }

    if (!shovelerModelRender(model)) {
      shovelerLogWarning(
          "Failed to render model %p for character %c when rendering text '%s' with material %p "
          "and model %p.",
          model,
          character,
          materialData->activeText,
          material,
          model);
      return false;
    }

    if (!material->screenspace) {
      shovelerRenderStateEnableDepthTest(renderState, GL_EQUAL);
    }

    materialData->activeCharacterAdvance += glyph->advance / 64.0f;
  }

  return true;
}

static void freeMaterialData(ShovelerMaterial* material) {
  MaterialData* materialData = material->data;

  shovelerSamplerFree(materialData->sampler);
  free(materialData);
}
