#include "shoveler/component/texture_sprite.h"

#include <assert.h>

#include "shoveler/client_system.h"
#include "shoveler/component/material.h"
#include "shoveler/component/sampler.h"
#include "shoveler/component/texture.h"
#include "shoveler/component_system.h"
#include "shoveler/log.h"
#include "shoveler/schema.h"
#include "shoveler/sprite/texture.h"
#include "shoveler/system.h"
#include "shoveler/texture.h"

static void* activateTextureSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer);
static void deactivateTextureSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer);

void shovelerClientSystemAddTextureSpriteSystem(ShovelerClientSystem* clientSystem) {
  ShovelerComponentType* componentType =
      shovelerSchemaGetComponentType(clientSystem->schema, shovelerComponentTypeIdTextureSprite);
  assert(componentType != NULL);

  ShovelerComponentSystem* componentSystem =
      shovelerSystemForComponentType(clientSystem->system, componentType);
  componentSystem->activateComponent = activateTextureSpriteComponent;
  componentSystem->deactivateComponent = deactivateTextureSpriteComponent;
  componentSystem->callbackUserData = clientSystem;
}

static void* activateTextureSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerComponent* materialComponent = shovelerComponentGetDependency(
      component, SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_MATERIAL);
  assert(materialComponent != NULL);
  ShovelerMaterial* material = shovelerComponentGetMaterial(materialComponent);
  assert(material != NULL);

  ShovelerComponent* textureComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_TEXTURE);
  assert(textureComponent != NULL);
  ShovelerTexture* texture = shovelerComponentGetTexture(textureComponent);
  assert(texture != NULL);

  ShovelerComponent* samplerComponent =
      shovelerComponentGetDependency(component, SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_SAMPLER);
  assert(samplerComponent != NULL);
  ShovelerSampler* sampler = shovelerComponentGetSampler(samplerComponent);
  assert(sampler != NULL);

  ShovelerSprite* textureSprite = shovelerSpriteTextureCreate(material, texture, sampler);

  ShovelerVector2 scale = shovelerComponentGetFieldValueVector2(
      component, SHOVELER_COMPONENT_TEXTURE_SPRITE_FIELD_ID_SCALE);

  shovelerSpriteUpdateSize(
      textureSprite,
      shovelerVector2(scale.values[0] * texture->width, scale.values[1] * texture->height));

  return textureSprite;
}

static void deactivateTextureSpriteComponent(
    ShovelerComponent* component, void* clientSystemPointer) {
  ShovelerSprite* TextureSprite = (ShovelerSprite*) component->systemData;
  assert(TextureSprite != NULL);

  shovelerSpriteFree(TextureSprite);
}
