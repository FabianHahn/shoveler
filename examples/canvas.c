#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/collider/box.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/material/canvas.h>
#include <shoveler/canvas.h>
#include <shoveler/collider.h>
#include <shoveler/colliders.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/framebuffer.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image.h>
#include <shoveler/input.h>
#include <shoveler/model.h>
#include <shoveler/opengl.h>
#include <shoveler/scene.h>
#include <shoveler/shader_program.h>
#include <shoveler/texture.h>
#include <shoveler/tile_sprite_animation.h>
#include <shoveler/tileset.h>

static void shovelerSampleTerminate();
static void shovelerSampleUpdate(ShovelerGame *game, double dt);

static ShovelerCanvasTileSprite characterSprite;
static ShovelerTileSpriteAnimation *animation;

int main(int argc, char *argv[])
{
	ShovelerGameWindowSettings windowSettings;
	windowSettings.windowTitle = "shoveler";
	windowSettings.fullscreen = false;
	windowSettings.vsync = true;
	windowSettings.samples = 4;
	windowSettings.windowedWidth = 640;
	windowSettings.windowedHeight = 480;

	ShovelerGameCameraSettings cameraSettings;
	cameraSettings.frame.position = shovelerVector3(0, 0, 1);
	cameraSettings.frame.direction = shovelerVector3(0, 0, -1);
	cameraSettings.frame.up = shovelerVector3(0, 1, 0);
	cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
	cameraSettings.projection.aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
	cameraSettings.projection.nearClippingPlane = 0.01;
	cameraSettings.projection.farClippingPlane = 1000;

	ShovelerGameControllerSettings controllerSettings;
	controllerSettings.frame = cameraSettings.frame;
	controllerSettings.moveFactor = 0.5f;
	controllerSettings.tiltFactor = 0.0005f;
	controllerSettings.boundingBoxSize2 = 0.2f;
	controllerSettings.boundingBoxSize3 = 0.0f;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerGame *game = shovelerGameCreate(shovelerSampleUpdate, /* updateAuthoritativeViewComponent */ NULL, &windowSettings, &cameraSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	game->controller->lockTiltX = true;
	game->controller->lockTiltY = true;

	ShovelerCanvas *canvas = shovelerCanvasCreate();

	ShovelerImage *tilesetImage = shovelerImageCreate(2, 2, 4);
	shovelerImageClear(tilesetImage);
	shovelerImageGet(tilesetImage, 0, 0, 0) = 255; // red
	shovelerImageGet(tilesetImage, 0, 0, 3) = 255;
	shovelerImageGet(tilesetImage, 0, 1, 1) = 255; // green
	shovelerImageGet(tilesetImage, 0, 1, 3) = 255;
	shovelerImageGet(tilesetImage, 1, 0, 2) = 255; // blue
	shovelerImageGet(tilesetImage, 1, 0, 3) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 0) = 255; // white
	shovelerImageGet(tilesetImage, 1, 1, 1) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 2) = 255;
	shovelerImageGet(tilesetImage, 1, 1, 3) = 255;
	ShovelerTileset *tileset = shovelerTilesetCreate(tilesetImage, 2, 2, 1);

	ShovelerImage *animationTilesetImage = shovelerImageCreateAnimationTileset(tilesetImage, 1);
	ShovelerTileset *animationTileset = shovelerTilesetCreate(animationTilesetImage, 4, 3, 1);

	shovelerImageFree(tilesetImage);
	shovelerImageFree(animationTilesetImage);

	ShovelerCanvasTileSprite tileSprite;
	tileSprite.tileset = tileset;
	tileSprite.tilesetColumn = 1;
	tileSprite.tilesetRow = 1;
	tileSprite.position = shovelerVector2(-0.3f, -0.2f);
	tileSprite.size = shovelerVector2(0.25f, 0.4f);
	shovelerCanvasAddTileSprite(canvas, &tileSprite);

	ShovelerCollider2 tileBoxCollider = shovelerColliderBox2(shovelerBoundingBox2(
		shovelerVector2LinearCombination(1.0f, tileSprite.position, -0.5f, tileSprite.size),
		shovelerVector2LinearCombination(1.0f, tileSprite.position, 0.5f, tileSprite.size)));
	shovelerCollidersAddCollider2(game->colliders, &tileBoxCollider);

	characterSprite.tileset = animationTileset;
	characterSprite.tilesetColumn = 0;
	characterSprite.tilesetRow = 0;
	characterSprite.position = shovelerVector2(0.0f, 0.0f);
	characterSprite.size = shovelerVector2(0.2f, 0.2f);
	shovelerCanvasAddTileSprite(canvas, &characterSprite);

	animation = shovelerTileSpriteAnimationCreate(&characterSprite, shovelerVector2(0.0f, 0.0f), 0.1f);
	animation->moveAmountThreshold = 0.25f;

	ShovelerMaterial *canvasMaterial = shovelerMaterialCanvasCreate(game->shaderCache, /* screenspace */ false);
	shovelerMaterialCanvasSetActive(canvasMaterial, canvas);
	shovelerMaterialCanvasSetActiveRegion(canvasMaterial, shovelerVector2(0.0f, 0.0f), shovelerVector2(1.0f, 1.0f));
	ShovelerDrawable *quad = shovelerDrawableQuadCreate();
	ShovelerModel *canvasModel = shovelerModelCreate(quad, canvasMaterial);
	canvasModel->scale = shovelerVector3(0.5, 0.5, 1.0);
	canvasModel->emitter = true;
	shovelerModelUpdateTransformation(canvasModel);
	shovelerSceneAddModel(game->scene, canvasModel);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerTileSpriteAnimationFree(animation);
	shovelerTilesetFree(tileset);
	shovelerTilesetFree(animationTileset);
	shovelerDrawableFree(quad);
	shovelerMaterialFree(canvasMaterial);
	shovelerCanvasFree(canvas);
	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void shovelerSampleUpdate(ShovelerGame *game, double dt)
{
	ShovelerController *controller = shovelerCameraPerspectiveGetController(game->camera);

	shovelerCameraUpdateView(game->camera);

	float moveAmountX = controller->frame.position.values[0] - characterSprite.position.values[0];
	float moveAmountY = controller->frame.position.values[1] - characterSprite.position.values[1];
	shovelerTileSpriteAnimationUpdate(animation, shovelerVector2(moveAmountX, moveAmountY));

	characterSprite.position.values[0] = controller->frame.position.values[0];
	characterSprite.position.values[1] = controller->frame.position.values[1];
}
