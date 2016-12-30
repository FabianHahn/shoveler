#include <glib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cameras/perspective.h"
#include "drawable/cube.h"
#include "drawable/quad.h"
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
static ShovelerMaterial *colorMaterial;
static ShovelerMaterial *screenspaceTextureMaterial;
static ShovelerMaterial *textureMaterial;
static ShovelerDrawable *quad;
static ShovelerDrawable *cube;
static ShovelerModel *cubeModel;
static ShovelerScene *scene;
static ShovelerFramebuffer *framebuffer;
static ShovelerCamera *camera;
static float previousCursorX;
static float previousCursorY;

static float eps = 1e-9;

void shovelerSampleInit(GLFWwindow *sampleWindow, int width, int height, int samples)
{
	window = sampleWindow;

	colorMaterial = shovelerMaterialColorCreate((ShovelerVector4){0.7, 0.7, 0.7, 1.0});

	ShovelerImage *image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255;
	ShovelerTexture *texture = shovelerTextureCreate2d(image);
	shovelerTextureUpdate(texture);
	ShovelerSampler *sampler = shovelerSamplerCreate(false, true);
	textureMaterial = shovelerMaterialTextureCreate(texture, sampler);

	shovelerOpenGLCheckSuccess();

	quad = shovelerDrawableQuadCreate();
	ShovelerModel *groundModel = shovelerModelCreate(quad, colorMaterial);
	groundModel->translation.values[0] = -10.0;
	groundModel->translation.values[1] = -5.0;
	groundModel->translation.values[2] = 10.0;
	groundModel->rotation.values[0] = SHOVELER_PI / 2.0f;
	groundModel->scale.values[0] = 20.0;
	groundModel->scale.values[1] = 20.0;
	shovelerModelUpdateTransformation(groundModel);

	cube = shovelerDrawableCubeCreate();
	cubeModel = shovelerModelCreate(cube, textureMaterial);

	scene = shovelerSceneCreate();
	shovelerSceneAddModel(scene, groundModel);
	shovelerSceneAddModel(scene, cubeModel);

	shovelerUniformMapInsert(scene->uniforms, "lightDirection", shovelerUniformCreateVector3(shovelerVector3Normalize((ShovelerVector3){0, 0, 1})));
	shovelerUniformMapInsert(scene->uniforms, "lightColor", shovelerUniformCreateVector4((ShovelerVector4){1, 1, 1, 1}));

	if(samples > 1) {
		framebuffer = shovelerFramebufferCreateMultisample(width, height, samples);
	} else {
		framebuffer = shovelerFramebufferCreate(width, height);
	}

	camera = shovelerCamerasPerspectiveCreate(2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);

	ShovelerCamera *lightCamera = shovelerCamerasPerspectiveCreate(2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);
	ShovelerLight *light = shovelerLightCreate(lightCamera, 1024, 1024);
	shovelerSceneAddLight(scene, light);

	screenspaceTextureMaterial = shovelerMaterialScreenspaceTextureCreate(texture, false, false, sampler, false);
	ShovelerModel *screenQuadModel = shovelerModelCreate(quad, screenspaceTextureMaterial);
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

	cubeModel->rotation.values[0] += 0.1f * dt;
	cubeModel->rotation.values[1] += 0.2f * dt;
	cubeModel->rotation.values[2] += 0.5f * dt;
	shovelerModelUpdateTransformation(cubeModel);

	shovelerSceneRender(scene, camera, framebuffer);
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
		shovelerCamerasPerspectiveTiltRight(camera, tiltAmountX);
		moved = true;
	}

	if(fabs(cursorDiffY) > eps) {
		shovelerCamerasPerspectiveTiltUp(camera, tiltAmountY);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_W);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveForward(camera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_S);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveForward(camera, -moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_A);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveRight(camera, -moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_D);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveRight(camera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_SPACE);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveUp(camera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveUp(camera, -moveAmount);
		moved = true;
	}

	if(moved) {
		shovelerLogTrace("Camera at position (%f, %f, %f)", camera->position.values[0], camera->position.values[1], camera->position.values[2]);
		shovelerCamerasPerspectiveUpdateTransformation(camera);
	}

	previousCursorX = newCursorX;
	previousCursorY = newCursorY;
}
