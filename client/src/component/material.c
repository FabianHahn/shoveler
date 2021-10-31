#include "shoveler/component/material.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/canvas.h"
#include "shoveler/component/resource.h"
#include "shoveler/component/sampler.h"
#include "shoveler/component/texture.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/material.h"
#include "shoveler/material/canvas.h"
#include "shoveler/material/color.h"
#include "shoveler/material/particle.h"
#include "shoveler/material/text.h"
#include "shoveler/material/texture.h"
#include "shoveler/material/texture_sprite.h"
#include "shoveler/material/tile_sprite.h"
#include "shoveler/material/tilemap.h"
#include "shoveler/schema.h"
#include "shoveler/system.h"

static void* activateMaterialComponent(ShovelerComponent* component, void* clientSystemPointer);
static void deactivateMaterialComponent(ShovelerComponent* component, void* clientSystemPointer);
static bool validateOptionalField(
    ShovelerComponent* component,
    ShovelerComponentMaterialFieldId configurationOption,
    const char* typeDescription);

void shovelerClientSystemAddMaterialSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdMaterial);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateMaterialComponent;
  componentSystem->deactivateComponent = deactivateMaterialComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateMaterialComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerClientSystem* clientSystem = clientSystemPointer;

  ShovelerComponentMaterialType type =
      shovelerComponentGetFieldValueInt(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TYPE);
  ShovelerMaterial* material;
  switch (type) {
  case SHOVELER_COMPONENT_MATERIAL_TYPE_COLOR: {
    if (!validateOptionalField(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR, "color")) {
      return NULL;
    }

    ShovelerVector4 color = shovelerComponentGetFieldValueVector4(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR);
    material =
        shovelerMaterialColorCreate(clientSystem->shaderCache, /* screenspace */ false, color);
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE: {
    if (!validateOptionalField(
            component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE, "texture")) {
      return NULL;
    }
    if (!validateOptionalField(
            component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SAMPLER, "texture")) {
      return NULL;
    }
    if (!validateOptionalField(
            component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_TYPE, "texture")) {
      return NULL;
    }

    ShovelerMaterialTextureType textureType = shovelerComponentGetFieldValueInt(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_TYPE);
    if (textureType == SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK) {
      if (!validateOptionalField(
              component,
              SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR,
              "texture with texture type alpha mask")) {
        return NULL;
      }
    }

    ShovelerComponent* textureComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE);
    assert(textureComponent != NULL);
    ShovelerTexture* texture = shovelerComponentGetTexture(textureComponent);
    assert(texture != NULL);

    ShovelerComponent* textureSamplerComponent = shovelerComponentGetDependency(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SAMPLER);
    assert(textureSamplerComponent != NULL);
    ShovelerSampler* sampler = shovelerComponentGetSampler(textureSamplerComponent);
    assert(sampler != NULL);

    material = shovelerMaterialTextureCreate(
        clientSystem->shaderCache,
        /* screenspace */ false,
        textureType,
        texture,
        false,
        sampler,
        false);

    if (textureType == SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK) {
      ShovelerVector4 color = shovelerComponentGetFieldValueVector4(
          component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR);
      shovelerMaterialTextureSetAlphaMaskColor(material, color);
    }
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_PARTICLE: {
    if (!validateOptionalField(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR, "particle")) {
      return NULL;
    }

    ShovelerVector4 color = shovelerComponentGetFieldValueVector4(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR);
    material = shovelerMaterialParticleCreate(clientSystem->shaderCache, color);
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_TILEMAP: {
    material = shovelerMaterialTilemapCreate(clientSystem->shaderCache, /* screenspace */ false);

    bool hasTilemap =
        shovelerComponentHasFieldValue(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TILEMAP);
    if (hasTilemap) {
      ShovelerComponent* tilemapComponent =
          shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TILEMAP);
      assert(tilemapComponent != NULL);
      ShovelerTilemap* tilemap = shovelerComponentGetTilemap(tilemapComponent);
      assert(tilemap != NULL);
      shovelerMaterialTilemapSetActive(material, tilemap);
    }
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_CANVAS: {
    if (!validateOptionalField(
            component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION, "canvas")) {
      return NULL;
    }
    if (!validateOptionalField(
            component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE, "canvas")) {
      return NULL;
    }
    if (!validateOptionalField(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS, "canvas")) {
      return NULL;
    }

    ShovelerVector2 canvasRegionPosition = shovelerComponentGetFieldValueVector2(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_POSITION);
    ShovelerVector2 canvasRegionSize = shovelerComponentGetFieldValueVector2(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS_REGION_SIZE);
    ShovelerComponent* canvasComponent =
        shovelerComponentGetDependency(component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_CANVAS);
    assert(canvasComponent != NULL);
    ShovelerCanvas* canvas = shovelerComponentGetCanvas(canvasComponent);
    assert(canvas != NULL);

    material = shovelerMaterialCanvasCreate(clientSystem->shaderCache, /* screenspace */ false);
    shovelerMaterialCanvasSetActive(material, canvas);
    shovelerMaterialCanvasSetActiveRegion(material, canvasRegionPosition, canvasRegionSize);
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE_SPRITE: {
    if (!validateOptionalField(
            component,
            SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SPRITE_TYPE,
            "texture sprite")) {
      return NULL;
    }

    ShovelerMaterialTextureSpriteType textureSpriteType = shovelerComponentGetFieldValueInt(
        component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_TEXTURE_SPRITE_TYPE);
    if (textureSpriteType == SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK) {
      if (!validateOptionalField(
              component,
              SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR,
              "texture sprite with texture type alpha mask")) {
        return NULL;
      }
    }

    material = shovelerMaterialTextureSpriteCreate(
        clientSystem->shaderCache, /* screenspace */ false, textureSpriteType);

    if (textureSpriteType == SHOVELER_MATERIAL_TEXTURE_SPRITE_TYPE_ALPHA_MASK) {
      ShovelerVector4 color = shovelerComponentGetFieldValueVector4(
          component, SHOVELER_COMPONENT_MATERIAL_FIELD_ID_COLOR);
      shovelerMaterialTextureSpriteSetColor(material, color);
    }
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_TILE_SPRITE: {
    material = shovelerMaterialTileSpriteCreate(clientSystem->shaderCache, /* screenspace */ false);
  } break;
  case SHOVELER_COMPONENT_MATERIAL_TYPE_TEXT: {
    material = shovelerMaterialTextCreate(clientSystem->shaderCache, /* screenspace */ false);
  } break;
  default:
    shovelerLogWarning(
        "Trying to activate material with unknown material type %d, ignoring.", type);
    return NULL;
  }

  return material;
}

static void deactivateMaterialComponent(ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerMaterial* material = (ShovelerMaterial*) component->systemData;

  shovelerMaterialFree(material);
}

static bool validateOptionalField(
    ShovelerComponent* component,
    ShovelerComponentMaterialFieldId configurationFieldId,
    const char* typeDescription) {
  assert(configurationFieldId < component->type->numFields);

  const ShovelerComponentField* field = &component->type->fields[configurationFieldId];
  assert(field->isOptional);

  if (!shovelerComponentHasFieldValue(component, configurationFieldId)) {
    shovelerLogWarning(
        "Failed to activate material component of entity %lld: Type is set to %s, but no %s option "
        "is provided.",
        component->entityId,
        typeDescription,
        field->dependencyComponentTypeId);
    return false;
  }

  return true;
}
