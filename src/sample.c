#include <glib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera/perspective.h"
#include "drawable/cube.h"
#include "drawable/quad.h"
#include "light/spot.h"
#include "material/color.h"
#include "material/screenspace_texture.h"
#include "material/texture.h"
#include "constants.h"
#include "framebuffer.h"
#include "image.h"
#include "input.h"
#include "model.h"
#include "opengl.h"
#include "sample.h"
#include "sampler.h"
#include "scene.h"
#include "shader_program.h"
#include "texture.h"

static void handleMovement(float dt);

static GLFWwindow *window;
static ShovelerSampler *nearestNeighborSampler;
static ShovelerSampler *interpolatingSampler;
static ShovelerMaterial *colorMaterial;
static ShovelerMaterial *screenspaceTextureMaterial;
static ShovelerMaterial *textureMaterial;
static ShovelerDrawable *quad;
static ShovelerDrawable *cube;
static ShovelerModel *downCubeModel;
static ShovelerModel *frontCubeModel;
static ShovelerModel *leftCubeModel;
static ShovelerModel *rightCubeModel;
static ShovelerScene *scene;
static ShovelerFramebuffer *framebuffer;
static ShovelerCamera *camera;
static float previousCursorX;
static float previousCursorY;

static float eps = 1e-9;

void shovelerSampleInit(GLFWwindow *sampleWindow, int width, int height, int samples)
{
	window = sampleWindow;

	nearestNeighborSampler = shovelerSamplerCreate(false, true);
	interpolatingSampler = shovelerSamplerCreate(true, true);

	colorMaterial = shovelerMaterialColorCreate((ShovelerVector4){0.7, 0.7, 0.7, 1.0});

	ShovelerImage *image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255;
	shovelerImageGet(image, 1, 1, 0) = 255;
	shovelerImageGet(image, 1, 1, 1) = 255;
	shovelerImageGet(image, 1, 1, 2) = 255;
	ShovelerTexture *texture = shovelerTextureCreate2d(image);
	shovelerTextureUpdate(texture);
	textureMaterial = shovelerMaterialTextureCreate(texture, true, nearestNeighborSampler, false);

	shovelerOpenGLCheckSuccess();

	scene = shovelerSceneCreate();
	quad = shovelerDrawableQuadCreate();

	ShovelerModel *groundModel = shovelerModelCreate(quad, colorMaterial);
	groundModel->translation.values[1] = -10.0f;
	groundModel->rotation.values[0] = SHOVELER_PI / 2.0f;
	groundModel->scale.values[0] = 10.0f;
	groundModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(groundModel);
	shovelerSceneAddModel(scene, groundModel);

	ShovelerModel *ceilingModel = shovelerModelCreate(quad, colorMaterial);
	ceilingModel->translation.values[1] = 10.0f;
	ceilingModel->rotation.values[0] = -SHOVELER_PI / 2.0f;
	ceilingModel->scale.values[0] = 10.0f;
	ceilingModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(ceilingModel);
	shovelerSceneAddModel(scene, ceilingModel);

	ShovelerModel *frontWallModel = shovelerModelCreate(quad, colorMaterial);
	frontWallModel->translation.values[2] = 10.0f;
	frontWallModel->rotation.values[0] = SHOVELER_PI;
	frontWallModel->scale.values[0] = 10.0f;
	frontWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(frontWallModel);
	shovelerSceneAddModel(scene, frontWallModel);

	ShovelerModel *backWallModel = shovelerModelCreate(quad, colorMaterial);
	backWallModel->translation.values[2] = -10.0f;
	backWallModel->scale.values[0] = 10.0f;
	backWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(backWallModel);
	shovelerSceneAddModel(scene, backWallModel);

	ShovelerModel *leftWallModel = shovelerModelCreate(quad, colorMaterial);
	leftWallModel->translation.values[0] = 10.0f;
	leftWallModel->rotation.values[1] = SHOVELER_PI / 2.0f;
	leftWallModel->scale.values[0] = 10.0f;
	leftWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(leftWallModel);
	shovelerSceneAddModel(scene, leftWallModel);

	ShovelerModel *rightWallModel = shovelerModelCreate(quad, colorMaterial);
	rightWallModel->translation.values[0] = -10.0f;
	rightWallModel->rotation.values[1] = -SHOVELER_PI / 2.0f;
	rightWallModel->scale.values[0] = 10.0f;
	rightWallModel->scale.values[1] = 10.0f;
	shovelerModelUpdateTransformation(rightWallModel);
	shovelerSceneAddModel(scene, rightWallModel);

	cube = shovelerDrawableCubeCreate();
	downCubeModel = shovelerModelCreate(cube, textureMaterial);
	downCubeModel->translation.values[1] = -5;
	shovelerModelUpdateTransformation(downCubeModel);
	shovelerSceneAddModel(scene, downCubeModel);

	frontCubeModel = shovelerModelCreate(cube, textureMaterial);
	frontCubeModel->translation.values[2] = 5;
	shovelerModelUpdateTransformation(frontCubeModel);
	shovelerSceneAddModel(scene, frontCubeModel);

	leftCubeModel = shovelerModelCreate(cube, textureMaterial);
	leftCubeModel->translation.values[0] = 5;
	shovelerModelUpdateTransformation(leftCubeModel);
	shovelerSceneAddModel(scene, leftCubeModel);

	rightCubeModel = shovelerModelCreate(cube, textureMaterial);
	rightCubeModel->translation.values[0] = -5;
	shovelerModelUpdateTransformation(rightCubeModel);
	shovelerSceneAddModel(scene, rightCubeModel);

	framebuffer = shovelerFramebufferCreate(width, height, samples, 4, 8);

	camera = shovelerCameraPerspectiveCreate((ShovelerVector3){0, 0, -5}, (ShovelerVector3){0, 0, 1}, (ShovelerVector3){0, 1, 0}, 2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);

	ShovelerCamera *lightCamera = shovelerCameraPerspectiveCreate((ShovelerVector3){0, 5, -5}, (ShovelerVector3){0, -5, 5}, (ShovelerVector3){0, 1, 0}, 2.0f * SHOVELER_PI * 50.0f / 360.0f, 1.0f, 1, 100);
	ShovelerLightSpot *spotlight = shovelerLightSpotCreate(lightCamera, 1024, 1024, 1, 0.0f, 80.0f, (ShovelerVector4){1.0f, 1.0f, 1.0f, 1.0f});
	shovelerSceneAddLight(scene, &spotlight->light);

	ShovelerCamera *lightCamera2 = shovelerCameraPerspectiveCreate((ShovelerVector3){0, 10, 0}, (ShovelerVector3){0, -1, 0}, (ShovelerVector3){0, 0, 1}, 2.0f * SHOVELER_PI * 50.0f / 360.0f, 1.0f, 1, 100);
	ShovelerLightSpot *spotlight2 = shovelerLightSpotCreate(lightCamera2, 512, 512, 1, 0.1f, 80.0f, (ShovelerVector4){0.0f, 0.0f, 0.3f, 1.0f});
	shovelerSceneAddLight(scene, &spotlight2->light);

	screenspaceTextureMaterial = shovelerMaterialScreenspaceTextureCreate(spotlight->shared->depthFramebuffer->depthTarget, false, true, nearestNeighborSampler, false);
	ShovelerModel *screenQuadModel = shovelerModelCreate(quad, screenspaceTextureMaterial);
	screenQuadModel->screenspace = true;
	screenQuadModel->translation.values[0] = -1.0;
	screenQuadModel->translation.values[1] = -1.0;
	screenQuadModel->scale.values[0] = 0.5;
	screenQuadModel->scale.values[1] = 0.5;
	shovelerModelUpdateTransformation(screenQuadModel);
	shovelerSceneAddModel(scene, screenQuadModel);

	shovelerOpenGLCheckSuccess();

	double newCursorX;
	double newCursorY;
	glfwGetCursorPos(window, &newCursorX, &newCursorY);
	previousCursorX = newCursorX;
	previousCursorY = newCursorY;
}

void shovelerSampleRender(float dt)
{
	handleMovement(dt);

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

	shovelerSceneRenderFrame(scene, camera, framebuffer);
	shovelerFramebufferBlitToDefault(framebuffer);
}

void shovelerSampleTerminate()
{
	shovelerFramebufferFree(framebuffer);
	shovelerSceneFree(scene);
	shovelerCameraFree(camera);
	shovelerDrawableFree(cube);
	shovelerMaterialFree(colorMaterial);
	shovelerMaterialFree(screenspaceTextureMaterial);
	shovelerMaterialFree(textureMaterial);
	shovelerSamplerFree(interpolatingSampler);
	shovelerSamplerFree(nearestNeighborSampler);
}

static void handleMovement(float dt)
{
	bool moved = false;
	float moveFactor = 2.0f;
	float moveAmount = moveFactor * dt;
	int state;

	double newCursorX;
	double newCursorY;
	glfwGetCursorPos(window, &newCursorX, &newCursorY);

	float cursorDiffX = newCursorX - previousCursorX;
	float cursorDiffY = newCursorY - previousCursorY;

	float tiltFactor = 0.0005f;
	float tiltAmountX = tiltFactor * cursorDiffX;
	float tiltAmountY = tiltFactor * cursorDiffY;

	if(fabs(cursorDiffX) > eps) {
		shovelerCameraPerspectiveTiltRight(camera, tiltAmountX);
		moved = true;
	}

	if(fabs(cursorDiffY) > eps) {
		shovelerCameraPerspectiveTiltUp(camera, tiltAmountY);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_W);
	if(state == GLFW_PRESS) {
		shovelerCameraPerspectiveMoveForward(camera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_S);
	if(state == GLFW_PRESS) {
		shovelerCameraPerspectiveMoveForward(camera, -moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_A);
	if(state == GLFW_PRESS) {
		shovelerCameraPerspectiveMoveRight(camera, -moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_D);
	if(state == GLFW_PRESS) {
		shovelerCameraPerspectiveMoveRight(camera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_SPACE);
	if(state == GLFW_PRESS) {
		shovelerCameraPerspectiveMoveUp(camera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	if(state == GLFW_PRESS) {
		shovelerCameraPerspectiveMoveUp(camera, -moveAmount);
		moved = true;
	}

	if(moved) {
		shovelerLogTrace("Camera at position (%f, %f, %f)", camera->position.values[0], camera->position.values[1], camera->position.values[2]);
		shovelerCameraPerspectiveUpdateTransformation(camera);
	}

	previousCursorX = newCursorX;
	previousCursorY = newCursorY;
}
