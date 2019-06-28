#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/drawable/cube.h>
#include <shoveler/drawable/quad.h>
#include <shoveler/drawable/point.h>
#include <shoveler/light/point.h>
#include <shoveler/material/color.h>
#include <shoveler/material/particle.h>
#include <shoveler/material/screenspace_texture.h>
#include <shoveler/material/texture.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/framebuffer.h>
#include <shoveler/game.h>
#include <shoveler/global.h>
#include <shoveler/image.h>
#include <shoveler/input.h>
#include <shoveler/model.h>
#include <shoveler/opengl.h>
#include <shoveler/sampler.h>
#include <shoveler/scene.h>
#include <shoveler/shader_program.h>
#include <shoveler/texture.h>

static ShovelerSampler *nearestNeighborSampler;
static ShovelerSampler *interpolatingSampler;
static ShovelerMaterial *colorMaterial;
static ShovelerMaterial *particleMaterial;
static ShovelerMaterial *screenspaceTextureMaterial;
static ShovelerMaterial *textureMaterial;
static ShovelerDrawable *quad;
static ShovelerDrawable *cube;
static ShovelerDrawable *point;
static ShovelerModel *downCubeModel;
static ShovelerModel *frontCubeModel;
static ShovelerModel *leftCubeModel;
static ShovelerModel *rightCubeModel;

static void shovelerSampleTerminate();
static void shovelerSampleUpdate(ShovelerGame *game, double dt);

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
	cameraSettings.frame.position = shovelerVector3(0, 0, -5);
	cameraSettings.frame.direction = shovelerVector3(0, 0, 1);
	cameraSettings.frame.up = shovelerVector3(0, 1, 0);
	cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
	cameraSettings.projection.aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
	cameraSettings.projection.nearClippingPlane = 0.01;
	cameraSettings.projection.farClippingPlane = 1000;

	ShovelerGameControllerSettings controllerSettings;
	controllerSettings.frame = cameraSettings.frame;
	controllerSettings.moveFactor = 2.0f;
	controllerSettings.tiltFactor = 0.0005f;
	controllerSettings.boundingBoxSize2 = 0.0f;
	controllerSettings.boundingBoxSize3 = 0.0f;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerGame *game = shovelerGameCreate(shovelerSampleUpdate, &windowSettings, &cameraSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	nearestNeighborSampler = shovelerSamplerCreate(false, true, true);
	interpolatingSampler = shovelerSamplerCreate(true, true, true);

	colorMaterial = shovelerMaterialColorCreate(game->shaderCache, shovelerVector3(0.7, 0.7, 0.7));

	ShovelerImage *image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255;
	shovelerImageGet(image, 1, 1, 0) = 255;
	shovelerImageGet(image, 1, 1, 1) = 255;
	shovelerImageGet(image, 1, 1, 2) = 255;
	ShovelerTexture *texture = shovelerTextureCreate2d(image, true);
	shovelerTextureUpdate(texture);
	textureMaterial = shovelerMaterialTextureCreate(game->shaderCache, texture, true, nearestNeighborSampler, false);

	shovelerOpenGLCheckSuccess();

	quad = shovelerDrawableQuadCreate();

	ShovelerModel *groundModel = shovelerModelCreate(quad, colorMaterial);
	groundModel->translation.values[1] = -10.0f;
	groundModel->rotation.values[0] = SHOVELER_PI / 2.0f;
	groundModel->scale.values[0] = 10.0f;
	groundModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(groundModel);
	shovelerSceneAddModel(game->scene, groundModel);

	ShovelerModel *ceilingModel = shovelerModelCreate(quad, colorMaterial);
	ceilingModel->translation.values[1] = 10.0f;
	ceilingModel->rotation.values[0] = -SHOVELER_PI / 2.0f;
	ceilingModel->scale.values[0] = 10.0f;
	ceilingModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(ceilingModel);
	shovelerSceneAddModel(game->scene, ceilingModel);

	ShovelerModel *frontWallModel = shovelerModelCreate(quad, colorMaterial);
	frontWallModel->translation.values[2] = 10.0f;
	frontWallModel->rotation.values[0] = SHOVELER_PI;
	frontWallModel->scale.values[0] = 10.0f;
	frontWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(frontWallModel);
	shovelerSceneAddModel(game->scene, frontWallModel);

	ShovelerModel *backWallModel = shovelerModelCreate(quad, colorMaterial);
	backWallModel->translation.values[2] = -10.0f;
	backWallModel->scale.values[0] = 10.0f;
	backWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(backWallModel);
	shovelerSceneAddModel(game->scene, backWallModel);

	ShovelerModel *leftWallModel = shovelerModelCreate(quad, colorMaterial);
	leftWallModel->translation.values[0] = 10.0f;
	leftWallModel->rotation.values[1] = SHOVELER_PI / 2.0f;
	leftWallModel->scale.values[0] = 10.0f;
	leftWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(leftWallModel);
	shovelerSceneAddModel(game->scene, leftWallModel);

	ShovelerModel *rightWallModel = shovelerModelCreate(quad, colorMaterial);
	rightWallModel->translation.values[0] = -10.0f;
	rightWallModel->rotation.values[1] = -SHOVELER_PI / 2.0f;
	rightWallModel->scale.values[0] = 10.0f;
	rightWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(rightWallModel);
	shovelerSceneAddModel(game->scene, rightWallModel);

	cube = shovelerDrawableCubeCreate();
	downCubeModel = shovelerModelCreate(cube, textureMaterial);
	downCubeModel->translation.values[1] = -5;
	shovelerModelUpdateTransformation(downCubeModel);
	shovelerSceneAddModel(game->scene, downCubeModel);

	frontCubeModel = shovelerModelCreate(cube, textureMaterial);
	frontCubeModel->translation.values[2] = 5;
	shovelerModelUpdateTransformation(frontCubeModel);
	shovelerSceneAddModel(game->scene, frontCubeModel);

	leftCubeModel = shovelerModelCreate(cube, textureMaterial);
	leftCubeModel->translation.values[0] = 5;
	shovelerModelUpdateTransformation(leftCubeModel);
	shovelerSceneAddModel(game->scene, leftCubeModel);

	rightCubeModel = shovelerModelCreate(cube, textureMaterial);
	rightCubeModel->translation.values[0] = -5;
	shovelerModelUpdateTransformation(rightCubeModel);
	shovelerSceneAddModel(game->scene, rightCubeModel);

	ShovelerLight *pointlight = shovelerLightPointCreate(game->shaderCache, shovelerVector3(0, 0, 0), 1024, 1024, 1, 0.0f, 80.0f, shovelerVector3(1.0f, 1.0f, 1.0f));
	shovelerSceneAddLight(game->scene, pointlight);

	ShovelerLight *pointlight2 = shovelerLightPointCreate(game->shaderCache, shovelerVector3(-8, -8, -8), 1024, 1024, 1, 0.0f, 80.0f, shovelerVector3(0.1f, 0.1f, 0.1f));
	shovelerSceneAddLight(game->scene, pointlight2);

	point = shovelerDrawablePointCreate();
	particleMaterial = shovelerMaterialParticleCreate(game->shaderCache, shovelerVector3(1.0f, 1.0f, 1.0f));
	ShovelerModel *pointlightModel = shovelerModelCreate(point, particleMaterial);
	pointlightModel->scale = shovelerVector3(0.5f, 0.5f, 0);
	pointlightModel->castsShadow = false;
	pointlightModel->emitter = true;
	shovelerModelUpdateTransformation(pointlightModel);
	shovelerSceneAddModel(game->scene, pointlightModel);

	ShovelerModel *pointlightModel2 = shovelerModelCreate(point, particleMaterial);
	pointlightModel2->translation = shovelerLightGetPosition(pointlight2);
	pointlightModel2->scale = shovelerVector3(0.1f, 0.1f, 0);
	pointlightModel2->castsShadow = false;
	pointlightModel2->emitter = true;
	shovelerModelUpdateTransformation(pointlightModel2);
	shovelerSceneAddModel(game->scene, pointlightModel2);

	screenspaceTextureMaterial = shovelerMaterialScreenspaceTextureCreate(game->shaderCache, shovelerLightPointGetShared(pointlight)->depthFramebuffer->depthTarget, false, true, nearestNeighborSampler, false);
	ShovelerModel *screenQuadModel = shovelerModelCreate(quad, screenspaceTextureMaterial);
	screenQuadModel->screenspace = true;
	screenQuadModel->translation.values[0] = -1.0;
	screenQuadModel->translation.values[1] = -1.0;
	screenQuadModel->scale.values[0] = 0.5;
	screenQuadModel->scale.values[1] = 0.5;
	shovelerModelUpdateTransformation(screenQuadModel);
	shovelerSceneAddModel(game->scene, screenQuadModel);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerSampleTerminate();
	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void shovelerSampleTerminate()
{
	shovelerDrawableFree(cube);
	shovelerDrawableFree(quad);
	shovelerDrawableFree(point);
	shovelerMaterialFree(colorMaterial);
	shovelerMaterialFree(particleMaterial);
	shovelerMaterialFree(screenspaceTextureMaterial);
	shovelerMaterialFree(textureMaterial);
	shovelerSamplerFree(interpolatingSampler);
	shovelerSamplerFree(nearestNeighborSampler);
}

static void shovelerSampleUpdate(ShovelerGame *game, double dt)
{
	shovelerCameraUpdateView(game->camera);

	downCubeModel->rotation.values[0] += 0.1f * dt;
	downCubeModel->rotation.values[1] += 0.2f * dt;
	downCubeModel->rotation.values[2] += 0.5f * dt;
	shovelerModelUpdateTransformation(downCubeModel);

	frontCubeModel->rotation.values[0] += 0.1f * dt;
	frontCubeModel->rotation.values[1] += 0.2f * dt;
	frontCubeModel->rotation.values[2] += 0.3f * dt;
	shovelerModelUpdateTransformation(frontCubeModel);

	leftCubeModel->rotation.values[0] += 0.4f * dt;
	leftCubeModel->rotation.values[1] += 0.2f * dt;
	leftCubeModel->rotation.values[2] += 0.1f * dt;
	shovelerModelUpdateTransformation(leftCubeModel);

	rightCubeModel->rotation.values[0] += 0.5f * dt;
	rightCubeModel->rotation.values[1] += 0.2f * dt;
	rightCubeModel->rotation.values[2] += 0.3f * dt;
	shovelerModelUpdateTransformation(rightCubeModel);
}
