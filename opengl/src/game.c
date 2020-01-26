#include <stdlib.h> // malloc, free

#include <glad/glad.h>

#include "shoveler/camera/perspective.h"
#include "shoveler/drawable/quad.h"
#include "shoveler/material/canvas.h"
#include "shoveler/view/colliders.h"
#include "shoveler/view/controller.h"
#include "shoveler/view/scene.h"
#include "shoveler/view/shader_cache.h"
#include "shoveler/canvas.h"
#include "shoveler/colliders.h"
#include "shoveler/game.h"
#include "shoveler/global.h"
#include "shoveler/input.h"
#include "shoveler/model.h"
#include "shoveler/opengl.h"
#include "shoveler/scene.h"
#include "shoveler/shader_cache.h"

static void updateScreenspaceCanvasRegion(ShovelerGame *game);
static void keyHandler(ShovelerInput *input, int key, int scancode, int action, int mods, void *unused);
static gint64 elapsedNs(double dt);
static void printFps(void *gamePointer);
static void updateAuthoritativeComponent(ShovelerView *view, ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *gamePointer);
static void updateViewCounters(ShovelerGame *game);

ShovelerGame *shovelerGameCreate(ShovelerGameUpdateCallback *update, ShovelerGameUpdateAuthoritativeViewComponentFunction *updateAuthoritativeViewComponent, const ShovelerGameWindowSettings *windowSettings, const ShovelerGameCameraSettings *cameraSettings, const ShovelerGameControllerSettings *controllerSettings)
{
	ShovelerGame *game = malloc(sizeof(ShovelerGame));
	game->windowedWidth = windowSettings->windowedWidth;
	game->windowedHeight = windowSettings->windowedHeight;
	game->samples = windowSettings->samples;
	game->fullscreen = windowSettings->fullscreen;
	game->updateExecutor = shovelerExecutorCreateDirect();

	// request OpenGL4
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	int width = windowSettings->windowedWidth;
	int height = windowSettings->windowedHeight;
	GLFWmonitor *monitor = NULL;
	if(windowSettings->fullscreen) {
		monitor = glfwGetPrimaryMonitor();
		shovelerLogInfo("Using borderless fullscreen mode on primary monitor '%s'.", glfwGetMonitorName(monitor));

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		width = mode->width;
		height = mode->height;
	} else {
		shovelerLogInfo("Using windowed mode.");
	}

	game->window = glfwCreateWindow(width, height, windowSettings->windowTitle, monitor, NULL);
	if(game->window == NULL) {
		shovelerLogError("Failed to create glfw window.");
		free(game);
		return NULL;
	}

	glfwMakeContextCurrent(game->window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		shovelerLogError("Failed to initialize glad.");
		glfwDestroyWindow(game->window);
		free(game);
		return NULL;
	}

	shovelerLogInfo("Opened glfw window with name '%s', using OpenGL driver version '%s' by '%s' and GLSL version '%s'.", windowSettings->windowTitle, glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_SHADING_LANGUAGE_VERSION));

	if(!shovelerOpenGLCheckSuccess()) {
		glfwDestroyWindow(game->window);
		free(game);
		return NULL;
	}

	game->renderState.blend = true;
	game->renderState.blendSourceFactor = GL_ONE;
	game->renderState.blendDestinationFactor = GL_ZERO;
	game->renderState.depthTest = true;
	game->renderState.depthFunction = GL_LESS;
	game->renderState.depthMask = GL_TRUE;
	shovelerRenderStateReset(&game->renderState);

	glEnable(GL_CULL_FACE);

	glfwSwapInterval(windowSettings->vsync ? 1 : 0);

	if(!shovelerOpenGLCheckSuccess()) {
		glfwTerminate();
		glfwDestroyWindow(game->window);
		free(game);
		return NULL;
	}

	game->input = shovelerInputCreate(game);
	shovelerInputAddKeyCallback(game->input, keyHandler, NULL);

	game->framebuffer = shovelerFramebufferCreate(width, height, windowSettings->samples, 4, 8);
	game->shaderCache = shovelerShaderCacheCreate();
	game->scene = shovelerSceneCreate(game->shaderCache);
	game->camera = shovelerCameraPerspectiveCreate(game->shaderCache, &cameraSettings->frame, &cameraSettings->projection);
	game->colliders = shovelerCollidersCreate();
	game->controller = shovelerControllerCreate(game->window, game->input, game->colliders, &controllerSettings->frame, controllerSettings->moveFactor, controllerSettings->tiltFactor, controllerSettings->boundingBoxSize2, controllerSettings->boundingBoxSize3);
	game->view = shovelerViewCreate(updateAuthoritativeComponent, game);

	game->screenspaceCanvas = shovelerCanvasCreate(/* numLayers */ 1);
	game->screenspaceCanvasQuad = shovelerDrawableQuadCreate();
	game->screenspaceCanvasMaterial = shovelerMaterialCanvasCreate(game->shaderCache, /* screenspace */ true);
	shovelerMaterialCanvasSetActive(game->screenspaceCanvasMaterial, game->screenspaceCanvas);
	updateScreenspaceCanvasRegion(game);
	game->screenspaceCanvasModel = shovelerModelCreate(game->screenspaceCanvasQuad, game->screenspaceCanvasMaterial);
	shovelerSceneAddModel(game->scene, game->screenspaceCanvasModel);

	game->update = update;
	game->updateAuthoritativeViewComponent = updateAuthoritativeViewComponent;
	game->lastFrameTime = glfwGetTime();
	game->lastFpsPrintTime = game->lastFrameTime;
	game->framesSinceLastFpsPrint = 0;
	game->lastViewCounters.numEntities = 0;
	game->lastViewCounters.numComponents = 0;
	game->lastViewCounters.numComponentDependencies = 0;
	game->lastViewCounters.numActiveComponents = 0;
	game->lastViewCounters.numDelegatedComponents = 0;

	shovelerExecutorSchedulePeriodic(game->updateExecutor, 1000, 1000, printFps, game);

	shovelerCameraPerspectiveAttachController(game->camera, game->controller);

	shovelerViewSetColliders(game->view, game->colliders);
	shovelerViewSetController(game->view, game->controller);
	shovelerViewSetScene(game->view, game->scene);
	shovelerViewSetShaderCache(game->view, game->shaderCache);

	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	g_hash_table_insert(global->games, game->window, game);

	return game;
}

ShovelerGame *shovelerGameGetForWindow(GLFWwindow *window)
{
	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	return g_hash_table_lookup(global->games, window);
}

void shovelerGameToggleFullscreen(ShovelerGame *game)
{
	if(game->fullscreen) {
		shovelerLogInfo("Switching game %p window to windowed mode.", game);
		glfwSetWindowMonitor(game->window, NULL, 0, 0, game->windowedWidth, game->windowedHeight, GLFW_DONT_CARE);

		shovelerFramebufferFree(game->framebuffer, /* keepTargets */ false);
		game->framebuffer = shovelerFramebufferCreate(game->windowedWidth, game->windowedHeight, game->samples, 4, 8);
	} else {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		shovelerLogInfo("Switching game %p window to fullscreen mode on primary monitor '%s'.", game, glfwGetMonitorName(monitor));

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(game->window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		shovelerFramebufferFree(game->framebuffer, /* keepTargets */ false);
		game->framebuffer = shovelerFramebufferCreate(mode->width, mode->height, game->samples, 4, 8);
	}

	game->fullscreen = !game->fullscreen;
	updateScreenspaceCanvasRegion(game);
}

int shovelerGameRenderFrame(ShovelerGame *game)
{
	glfwMakeContextCurrent(game->window);

	double now = glfwGetTime();
	double dt = now - game->lastFrameTime;
	game->lastFrameTime = now;
	game->framesSinceLastFpsPrint++;

	shovelerExecutorUpdate(game->updateExecutor, elapsedNs(dt));
	shovelerControllerUpdate(game->controller, dt);
	game->update(game, dt);

	updateViewCounters(game);

	int rendered = shovelerSceneRenderFrame(game->scene, game->camera, game->framebuffer, &game->renderState);
	shovelerFramebufferBlitToDefault(game->framebuffer);

	glfwSwapBuffers(game->window);
	glfwPollEvents();

	return rendered;
}

void shovelerGameFree(ShovelerGame *game)
{
	ShovelerGlobalContext *global = shovelerGlobalGetContext();
	g_hash_table_remove(global->games, game->window);

	shovelerCameraPerspectiveDetachController(game->camera);

	shovelerFramebufferFree(game->framebuffer, /* keepTargets */ false);

	shovelerSceneRemoveModel(game->scene, game->screenspaceCanvasModel);
	shovelerMaterialFree(game->screenspaceCanvasMaterial);
	shovelerDrawableFree(game->screenspaceCanvasQuad);
	shovelerCanvasFree(game->screenspaceCanvas);

	shovelerViewFree(game->view);
	shovelerControllerFree(game->controller);
	shovelerCollidersFree(game->colliders);
	shovelerInputFree(game->input);
	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerShaderCacheFree(game->shaderCache);

	glfwDestroyWindow(game->window);

	shovelerExecutorFree(game->updateExecutor);
	free(game);
}

static void updateScreenspaceCanvasRegion(ShovelerGame *game)
{
	ShovelerVector2 regionPosition = shovelerVector2(0.5f * game->framebuffer->width, 0.5f * game->framebuffer->height);
	ShovelerVector2 regionSize = shovelerVector2(game->framebuffer->width, game->framebuffer->height);

	shovelerMaterialCanvasSetActiveRegion(game->screenspaceCanvasMaterial, regionPosition, regionSize);
}

static void keyHandler(ShovelerInput *input, int key, int scancode, int action, int mods, void *unused)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		shovelerLogInfo("Escape key pressed, terminating.");
		glfwSetWindowShouldClose(input->game->window, GLFW_TRUE);
	}

	if(key == GLFW_KEY_F9 && action == GLFW_PRESS) {
		shovelerLogInfo("F9 key pressed, writing view dependency graph.");
		shovelerViewWriteDependencyGraph(input->game->view, "view_dependencies.dot");
	}

	if(key == GLFW_KEY_F10 && action == GLFW_PRESS) {
		shovelerLogInfo("F10 key pressed, toggling scene debug mode.");
		shovelerSceneToggleDebugMode(input->game->scene);
	}
}

static gint64 elapsedNs(double dt)
{
	gint64 elapsed = dt * G_USEC_PER_SEC;
	if(elapsed > 0) {
		return elapsed;
	} else {
		return 0;
	}
}

static void printFps(void *gamePointer)
{
	ShovelerGame *game = gamePointer;
	double now = glfwGetTime();
	double secondsSinceLastFpsPrint = now - game->lastFpsPrintTime;

	double fps = game->framesSinceLastFpsPrint / secondsSinceLastFpsPrint;
	shovelerLogInfo("Current FPS: %.1f", fps);

	game->lastFpsPrintTime = now;
	game->framesSinceLastFpsPrint = 0;
}

static void updateAuthoritativeComponent(ShovelerView *view, ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *gamePointer)
{
	ShovelerGame *game = (ShovelerGame *) gamePointer;

	if(game->updateAuthoritativeViewComponent != NULL) {
		game->updateAuthoritativeViewComponent(game, component, configurationOption, value);
	}
}

static void updateViewCounters(ShovelerGame *game)
{
	bool viewCountersChanged = game->lastViewCounters.numEntities != game->view->numEntities
		|| game->lastViewCounters.numComponents != game->view->numComponents
		|| game->lastViewCounters.numComponentDependencies != game->view->numComponentDependencies
		|| game->lastViewCounters.numActiveComponents != game->view->numActiveComponents
		|| game->lastViewCounters.numDelegatedComponents != game->view->numDelegatedComponents;

	if(viewCountersChanged) {
		game->lastViewCounters.numEntities = game->view->numEntities;
		game->lastViewCounters.numComponents = game->view->numComponents;
		game->lastViewCounters.numComponentDependencies = game->view->numComponentDependencies;
		game->lastViewCounters.numActiveComponents = game->view->numActiveComponents;
		game->lastViewCounters.numDelegatedComponents = game->view->numDelegatedComponents;

		shovelerLogInfo("View changed: %u entities, %u components (%u dependencies, %u active, %u delegated).",
			game->lastViewCounters.numEntities,
			game->lastViewCounters.numComponents,
			game->lastViewCounters.numComponentDependencies,
			game->lastViewCounters.numActiveComponents,
			game->lastViewCounters.numDelegatedComponents);
	}
}
