#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/view/drawable.h>
#include <shoveler/view/light.h>
#include <shoveler/view/model.h>
#include <shoveler/view/position.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/global.h>
#include <shoveler/opengl.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

static void updateGame(ShovelerGame *game, double dt);

static double time = 0.0;
static ShovelerView *view = NULL;

int main(int argc, char *argv[])
{
	const char *windowTitle = "shoveler";
	bool fullscreen = false;
	bool vsync = true;
	int samples = 4;
	int width = 640;
	int height = 480;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerGame *game = shovelerGameCreate(windowTitle, width, height, samples, fullscreen, vsync);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	game->camera = shovelerCameraPerspectiveCreate(shovelerVector3(0.0, 0.0, -5.0), shovelerVector3(0.0, 0.0, 1.0), shovelerVector3(0.0, 1.0, 0.0), 2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);
	game->scene = shovelerSceneCreate();
	game->update = updateGame;

	ShovelerController *controller = shovelerControllerCreate(game, shovelerVector3(0.0, 0.0, -5.0), shovelerVector3(0.0, 0.0, 1.0), shovelerVector3(0.0, 1.0, 0.0), 2.0f, 0.0005f);
	shovelerCameraPerspectiveAttachController(game->camera, controller);

	view = shovelerViewCreate();
	shovelerViewSetTarget(view, "controller", controller);
	shovelerViewSetTarget(view, "scene", game->scene);

	ShovelerViewDrawableConfiguration cubeDrawableConfiguration;
	cubeDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_CUBE;
	ShovelerViewEntity *cubeDrawableEntity = shovelerViewAddEntity(view, 1);
	shovelerViewEntityAddDrawable(cubeDrawableEntity, cubeDrawableConfiguration);

	ShovelerViewModelConfiguration cubeModelConfiguration;
	cubeModelConfiguration.drawableEntityId = 1;
	cubeModelConfiguration.material.type = SHOVELER_VIEW_MATERIAL_TYPE_COLOR;
	cubeModelConfiguration.material.color = shovelerVector3(0.7, 0.7, 0.7);
	cubeModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	cubeModelConfiguration.scale = shovelerVector3(1.0, 1.0, 1.0);
	cubeModelConfiguration.visible = true;
	cubeModelConfiguration.emitter = false;
	cubeModelConfiguration.screenspace = false;
	cubeModelConfiguration.castsShadow = true;
	cubeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *cubeEntity = shovelerViewAddEntity(view, 2);
	shovelerViewEntityAddModel(cubeEntity, cubeModelConfiguration);
	shovelerViewEntityAddPosition(cubeEntity, 0.0, 0.0, 5.0);

	ShovelerViewDrawableConfiguration quadDrawableConfiguration;
	quadDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_QUAD;
	ShovelerViewModelConfiguration planeModelConfiguration;
	planeModelConfiguration.drawableEntityId = 3;
	planeModelConfiguration.material.type = SHOVELER_VIEW_MATERIAL_TYPE_COLOR;
	planeModelConfiguration.material.color = shovelerVector3(0.7, 0.7, 0.7);
	planeModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, 0.0);
	planeModelConfiguration.scale = shovelerVector3(10.0, 10.0, 1.0);
	planeModelConfiguration.visible = true;
	planeModelConfiguration.emitter = false;
	planeModelConfiguration.screenspace = false;
	planeModelConfiguration.castsShadow = true;
	planeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *planeEntity = shovelerViewAddEntity(view, 3);
	shovelerViewEntityAddDrawable(planeEntity, quadDrawableConfiguration);
	shovelerViewEntityAddModel(planeEntity, planeModelConfiguration);
	shovelerViewEntityAddPosition(planeEntity, 0.0, 0.0, 10.0);

	ShovelerViewDrawableConfiguration pointDrawableConfiguration;
	pointDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_POINT;
	ShovelerViewModelConfiguration lightModelConfiguration;
	lightModelConfiguration.drawableEntityId = 4;
	lightModelConfiguration.material.type = SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE;
	lightModelConfiguration.material.color = shovelerVector3(1.0, 1.0, 1.0);
	lightModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	lightModelConfiguration.scale = shovelerVector3(0.5, 0.5, 0.5);
	lightModelConfiguration.visible = true;
	lightModelConfiguration.emitter = true;
	lightModelConfiguration.screenspace = false;
	lightModelConfiguration.castsShadow = false;
	lightModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewLightConfiguration lightConfiguration;
	lightConfiguration.type = SHOVELER_VIEW_LIGHT_TYPE_POINT;
	lightConfiguration.width = 512;
	lightConfiguration.height = 512;
	lightConfiguration.samples = 1;
	lightConfiguration.ambientFactor = 0.0;
	lightConfiguration.exponentialFactor = 80.0;
	lightConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewEntity *lightEntity = shovelerViewAddEntity(view, 4);
	shovelerViewEntityAddDrawable(lightEntity, pointDrawableConfiguration);
	shovelerViewEntityAddModel(lightEntity, lightModelConfiguration);
	shovelerViewEntityAddLight(lightEntity, lightConfiguration);
	shovelerViewEntityAddPosition(lightEntity, 0.0, 2.0, 0.0);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerViewFree(view);
	shovelerCameraPerspectiveDetachController(game->camera);
	shovelerControllerFree(controller);
	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void updateGame(ShovelerGame *game, double dt)
{
	shovelerControllerUpdate(shovelerCameraPerspectiveGetController(game->camera), dt);
	shovelerCameraUpdateView(game->camera);

	time += dt;
	ShovelerViewEntity *lightEntity = shovelerViewGetEntity(view, 4);
	shovelerViewEntityUpdatePosition(lightEntity, 2.0 * sin(time), 2.0 * cos(time), 0.0);
}
