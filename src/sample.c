#include <glib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cameras/perspective.h"
#include "drawables/cube.h"
#include "constants.h"
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

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform mat4 model;\n"
		"uniform mat4 modelNormal;\n"
		"uniform mat4 camera;\n"
		"\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec2 uv;\n"
		"\n"
		"out vec3 worldPosition;"
		"out vec3 worldNormal;"
		"out vec2 worldUv;"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 worldPosition4 = model * vec4(position, 1.0);\n"
		"	vec4 worldNormal4 = modelNormal * vec4(normal, 1.0);\n"
		"	worldPosition = worldPosition4.xyz / worldPosition4.w;\n"
		"	worldNormal = worldNormal4.xyz / worldNormal4.w;\n"
		"	worldUv = uv;\n"
		"	gl_Position = camera * worldPosition4;\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"uniform vec3 lightDirection;\n"
		"uniform vec3 lightColor;\n"
		"uniform vec3 cameraPosition;\n"
		"uniform sampler2D textureImage;\n"
		"\n"
		"in vec3 worldPosition;\n"
		"in vec3 worldNormal;\n"
		"in vec2 worldUv;\n"
		"\n"
		"out vec4 fragmentColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec3 color = texture2D(textureImage, worldUv).rgb;\n"
		"	vec3 normal = normalize(worldNormal);\n"
		"	"
		"	float ambientFactor = 0.2;\n"
		"	float diffuseFactor = clamp(dot(-lightDirection, normal), 0.0, 1.0);\n"
		""
		"	vec3 cameraDirection = normalize(cameraPosition - worldPosition);\n"
		"	vec3 reflectedLight = -reflect(-lightDirection, normal);\n"
		"	float specularFactor = pow(clamp(dot(reflectedLight, cameraDirection), 0.0, 1.0), 250.0);\n"
		""
		"	fragmentColor = vec4(clamp(ambientFactor * color + diffuseFactor * color + specularFactor * lightColor, 0.0, 1.0), 1.0);\n"
		"}\n";

static GLFWwindow *window;
static ShovelerMaterial *material;
static ShovelerTexture *texture;
static ShovelerSampler *sampler;
static ShovelerDrawable *cube;
static ShovelerModel *model;
static ShovelerScene *scene;
static ShovelerCamerasPerspective *perspectiveCamera;
static float previousCursorX;
static float previousCursorY;

static float eps = 1e-9;

void shovelerSampleInit(GLFWwindow *sampleWindow, int width, int height)
{
	window = sampleWindow;

	GLuint vertexShaderObject = shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderObject = shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint program = shovelerShaderProgramLink(vertexShaderObject, fragmentShaderObject, true);
	material = shovelerMaterialCreate(program);

	ShovelerImage *image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255;
	texture = shovelerTextureCreate2d(image);
	shovelerTextureUpdate(texture);
	sampler = shovelerSamplerCreate(false, true);
	shovelerMaterialAttachTexture(material, "textureImage", texture, sampler);

	ShovelerUniform *lightDirectionUniform = shovelerUniformCreateVector3(shovelerVector3Normalize((ShovelerVector3){0, 0, 1}));
	shovelerUniformMapInsert(material->uniforms, "lightDirection", lightDirectionUniform);
	ShovelerUniform *lightColorUniform = shovelerUniformCreateVector3((ShovelerVector3){1, 1, 1});
	shovelerUniformMapInsert(material->uniforms, "lightColor", lightColorUniform);

	shovelerOpenGLCheckSuccess();

	cube = shovelerDrawablesCubeCreate();
	model = shovelerModelCreate(cube, material);

	scene = shovelerSceneCreate();
	shovelerSceneAddModel(scene, model);

	perspectiveCamera = shovelerCamerasPerspectiveCreate(2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);

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

	model->rotation.values[0] += 0.1f * dt;
	model->rotation.values[1] += 0.2f * dt;
	model->rotation.values[2] += 0.5f * dt;
	shovelerModelUpdateTransformation(model);

	shovelerSceneRender(scene, perspectiveCamera->camera);
}

void shovelerSampleTerminate()
{
	shovelerSceneFree(scene);
	shovelerCamerasPerspectiveFree(perspectiveCamera);
	shovelerDrawableFree(cube);
	shovelerSamplerFree(sampler);
	shovelerTextureFree(texture);
	shovelerMaterialFree(material);
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
		shovelerCamerasPerspectiveTiltRight(perspectiveCamera, tiltAmountX);
		moved = true;
	}

	if(fabs(cursorDiffY) > eps) {
		shovelerCamerasPerspectiveTiltUp(perspectiveCamera, tiltAmountY);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_W);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveForward(perspectiveCamera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_S);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveForward(perspectiveCamera, -moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_A);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveRight(perspectiveCamera, -moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_D);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveRight(perspectiveCamera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveUp(perspectiveCamera, moveAmount);
		moved = true;
	}

	state = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	if(state == GLFW_PRESS) {
		shovelerCamerasPerspectiveMoveUp(perspectiveCamera, -moveAmount);
		moved = true;
	}

	if(moved) {
		shovelerLogTrace("Camera at position (%f, %f, %f)", perspectiveCamera->camera->position.values[0], perspectiveCamera->camera->position.values[1], perspectiveCamera->camera->position.values[2]);
		shovelerCamerasPerspectiveUpdateTransformation(perspectiveCamera);
	}

	previousCursorX = newCursorX;
	previousCursorY = newCursorY;
}
