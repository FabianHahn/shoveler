#include <glib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cameras/perspective.h"
#include "drawables/cube.h"
#include "input.h"
#include "model.h"
#include "opengl.h"
#include "sample.h"
#include "scene.h"
#include "shader_program.h"

static void handleMovement(float dt);

static const char *vertexShaderSource =
		"#version 400\n"
		"\n"
		"uniform mat4 model;\n"
		"uniform mat4 camera;\n"
		"\n"
		"in vec3 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = camera * model * vec4(position, 1.0);\n"
		"}\n";

static const char *fragmentShaderSource =
		"#version 400\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

static GLFWwindow *window;
static ShovelerMaterial *material;
static ShovelerDrawable *cube;
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
	shovelerOpenGLCheckSuccess();

	cube = shovelerDrawablesCubeCreate();
	ShovelerModel *model = shovelerModelCreate(cube, material);

	scene = shovelerSceneCreate();
	shovelerSceneAddModel(scene, model);

	perspectiveCamera = shovelerCamerasPerspectiveCreate(2.0f * G_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);

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
	shovelerSceneRender(scene, perspectiveCamera->camera);
}

void shovelerSampleTerminate()
{
	shovelerSceneFree(scene);
	shovelerCamerasPerspectiveFree(perspectiveCamera);
	shovelerDrawableFree(cube);
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

	float tiltFactor = 0.05f;
	float tiltAmountX = tiltFactor * cursorDiffX * dt;
	float tiltAmountY = tiltFactor * cursorDiffY * dt;

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
