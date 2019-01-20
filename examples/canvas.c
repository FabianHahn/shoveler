#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/material/canvas.h>
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

static void shovelerSampleInit(ShovelerGame *sampleGame, int width, int height, int samples);
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

	ShovelerVector3 position = {0, 0, 1};
	ShovelerVector3 direction = {0, 0, -1};
	ShovelerVector3 up = {0, 1, 0};
	float fov = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
	float aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerCamera *camera = shovelerCameraPerspectiveCreate(position, direction, up, fov, aspectRatio, 0.01, 1000);

	ShovelerGame *game = shovelerGameCreate(camera, shovelerSampleUpdate, &windowSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	ShovelerController *controller = shovelerControllerCreate(game, position, direction, up, 0.5f, 0.0005f);
	controller->lockTiltX = true;
	controller->lockTiltY = true;
	shovelerCameraPerspectiveAttachController(camera, controller);

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
	tileSprite.position = shovelerVector2(0.2f, 0.3f);
	tileSprite.size = shovelerVector2(0.25f, 0.4f);
	shovelerCanvasAddTileSprite(canvas, &tileSprite);

	characterSprite.tileset = animationTileset;
	characterSprite.tilesetColumn = 0;
	characterSprite.tilesetRow = 0;
	characterSprite.position = shovelerVector2(0.5f, 0.5f);
	characterSprite.size = shovelerVector2(0.2f, 0.2f);
	shovelerCanvasAddTileSprite(canvas, &characterSprite);

	animation = shovelerTileSpriteAnimationCreate(&characterSprite, 0.1f);
	animation->moveAmountThreshold = 0.25f;
	animation->logDirectionChanges = true;

	ShovelerMaterial *canvasMaterial = shovelerMaterialCanvasCreate();
	shovelerMaterialCanvasSetActive(canvasMaterial, canvas);
	shovelerMaterialCanvasSetActiveRegion(canvasMaterial, shovelerVector2(0.5f, 0.5f), shovelerVector2(1.0f, 1.0f));
	ShovelerDrawable *quad = shovelerDrawableQuadCreate();
	ShovelerModel *canvasModel = shovelerModelCreate(quad, canvasMaterial);
	canvasModel->scale = shovelerVector3(0.5, 0.5, 1.0);
	canvasModel->screenspace = true;
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
	shovelerCameraFree(camera);
	shovelerDrawableFree(quad);
	shovelerMaterialFree(canvasMaterial);
	shovelerCanvasFree(canvas);
	shovelerControllerFree(controller);

	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void shovelerSampleUpdate(ShovelerGame *game, double dt)
{
	ShovelerController *controller = shovelerCameraPerspectiveGetController(game->camera);

	shovelerControllerUpdate(controller, dt);
	shovelerCameraUpdateView(game->camera);

	float moveAmountX = controller->position.values[0] - characterSprite.position.values[0] + 0.5f;
	float moveAmountY = controller->position.values[1] - characterSprite.position.values[1] + 0.5f;
	shovelerTileSpriteAnimationUpdate(animation, shovelerVector2(moveAmountX, moveAmountY));

	characterSprite.position.values[0] = 0.5f + controller->position.values[0];
	characterSprite.position.values[1] = 0.5f + controller->position.values[1];
}
