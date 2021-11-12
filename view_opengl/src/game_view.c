#include "shoveler/game_view.h"

#include "shoveler/component/canvas.h"
#include "shoveler/component/client.h"
#include "shoveler/component/colliders.h"
#include "shoveler/component/controller.h"
#include "shoveler/component/drawable.h"
#include "shoveler/component/font.h"
#include "shoveler/component/font_atlas.h"
#include "shoveler/component/font_atlas_texture.h"
#include "shoveler/component/fonts.h"
#include "shoveler/component/image.h"
#include "shoveler/component/light.h"
#include "shoveler/component/material.h"
#include "shoveler/component/model.h"
#include "shoveler/component/position.h"
#include "shoveler/component/render_state.h"
#include "shoveler/component/resource.h"
#include "shoveler/component/sampler.h"
#include "shoveler/component/scene.h"
#include "shoveler/component/shader_cache.h"
#include "shoveler/component/sprite.h"
#include "shoveler/component/text_sprite.h"
#include "shoveler/component/text_texture_renderer.h"
#include "shoveler/component/texture.h"
#include "shoveler/component/tile_sprite.h"
#include "shoveler/component/tile_sprite_animation.h"
#include "shoveler/component/tilemap.h"
#include "shoveler/component/tilemap_colliders.h"
#include "shoveler/component/tilemap_sprite.h"
#include "shoveler/component/tilemap_tiles.h"
#include "shoveler/component/tileset.h"

ShovelerView* shovelerGameViewCreate(
	ShovelerGame* game,
	ShovelerViewUpdateAuthoritativeComponentFunction* updateAuthoritativeComponent,
	void* updateAuthoritativeComponentUserData) {
	ShovelerView* view =
		shovelerViewCreate(updateAuthoritativeComponent, updateAuthoritativeComponentUserData);

	shovelerViewSetTarget(view, shovelerComponentViewTargetIdColliders, game->colliders);
	shovelerViewSetTarget(view, shovelerComponentViewTargetIdController, game->controller);
	shovelerViewSetTarget(view, shovelerComponentViewTargetIdFonts, game->fonts);
	shovelerViewSetTarget(view, shovelerComponentViewTargetIdRenderState, &game->renderState);
	shovelerViewSetTarget(view, shovelerComponentViewTargetIdScene, game->scene);
	shovelerViewSetTarget(view, shovelerComponentViewTargetIdShaderCache, game->shaderCache);

	shovelerViewAddComponentType(view, shovelerComponentCreateCanvasType());
	shovelerViewAddComponentType(view, shovelerComponentCreateClientType());
	shovelerViewAddComponentType(view, shovelerComponentCreateDrawableType());
	shovelerViewAddComponentType(view, shovelerComponentCreateFontType());
	shovelerViewAddComponentType(view, shovelerComponentCreateFontAtlasType());
	shovelerViewAddComponentType(view, shovelerComponentCreateFontAtlasTextureType());
	shovelerViewAddComponentType(view, shovelerComponentCreateImageType());
	shovelerViewAddComponentType(view, shovelerComponentCreateLightType());
	shovelerViewAddComponentType(view, shovelerComponentCreateMaterialType());
	shovelerViewAddComponentType(view, shovelerComponentCreateModelType());
	shovelerViewAddComponentType(view, shovelerComponentCreatePositionType());
	shovelerViewAddComponentType(view, shovelerComponentCreateResourceType());
	shovelerViewAddComponentType(view, shovelerComponentCreateSamplerType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTextureType());
	shovelerViewAddComponentType(view, shovelerComponentCreateSpriteType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTextSpriteType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTextTextureRendererType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTileSpriteType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTileSpriteAnimationType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTilemapType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTilemapCollidersType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTilemapSpriteType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTilemapTilesType());
	shovelerViewAddComponentType(view, shovelerComponentCreateTilesetType());

	return view;
}
