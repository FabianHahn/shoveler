#ifndef SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_H
#define SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_H

typedef struct ShovelerTileSpriteAnimationStruct ShovelerTileSpriteAnimation; // forward declaration: tile_sprite_animation.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerComponentTypeNameTileSpriteAnimation = "tile_sprite_animation";
static const char *shovelerComponentTileSpriteAnimationOptionKeyPosition = "position";
static const char *shovelerComponentTileSpriteAnimationOptionKeyTileSprite = "tile_sprite";
static const char *shovelerComponentTileSpriteAnimationOptionKeyPositionMappingX = "position_mapping_x";
static const char *shovelerComponentTileSpriteAnimationOptionKeyPositionMappingY = "position_mapping_y";
static const char *shovelerComponentTileSpriteAnimationOptionKeyMoveAmountThreshold = "move_amount_threshold";

ShovelerComponentType *shovelerComponentCreateTileSpriteAnimationType();
ShovelerTileSpriteAnimation *shovelerComponentGetTileSpriteAnimation(ShovelerComponent *component);

#endif
