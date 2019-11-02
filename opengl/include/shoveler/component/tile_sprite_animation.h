#ifndef SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_H
#define SHOVELER_COMPONENT_TILE_SPRITE_ANIMATION_H

typedef struct ShovelerTileSpriteAnimationStruct ShovelerTileSpriteAnimation; // forward declaration: tile_sprite_animation.h
typedef struct ShovelerComponentTypeStruct ShovelerComponentType; // forward declaration: component.h
typedef struct ShovelerComponentStruct ShovelerComponent; // forward declaration: component.h

static const char *shovelerViewTileSpriteAnimationComponentTypeName = "tile_sprite_animation";
static const char *shovelerViewTileSpriteAnimationPositionOptionKey = "position";
static const char *shovelerViewTileSpriteAnimationTileSpriteOptionKey = "tile_sprite";
static const char *shovelerViewTileSpriteAnimationPositionMappingXOptionKey = "position_mapping_x";
static const char *shovelerViewTileSpriteAnimationPositionMappingYOptionKey = "position_mapping_y";
static const char *shovelerViewTileSpriteAnimationMoveAmountThresholdOptionKey = "move_amount_threshold";

ShovelerComponentType *shovelerComponentCreateTileSpriteAnimationType();
ShovelerTileSpriteAnimation *shovelerComponentGetTileSpriteAnimation(ShovelerComponent *component);

#endif
