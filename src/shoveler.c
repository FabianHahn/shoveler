#include <stdio.h> // stdout
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <stdbool.h> // bool

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "log.h"
#include "opengl.h"
#include "sample.h"

static void exitKeyHandler(int key, int scancode, int action, int mods);

static GLFWwindow *window;

int main(int argc, char *argv[])
{
	ShovelerLogLevel logLevel = SHOVELER_LOG_LEVEL_ALL;
	FILE *logChannel = stdout;
	const char *windowTitle = "shoveler";
	bool fullscreen = false;
	bool vsync = true;
	int width = 640;
	int height = 480;

	shovelerLogInit(logLevel, logChannel);

	if(!glfwInit()) {
		shovelerLogError("Failed to initialize glfw.");
		shovelerLogTerminate();
		return EXIT_FAILURE;
	}

	// request OpenGL4
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	if(fullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		shovelerLogInfo("Using borderless fullscreen mode on primary monitor '%s'.", glfwGetMonitorName(monitor));

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		width = mode->width;
		height = mode->height;
		window = glfwCreateWindow(width, height, windowTitle, monitor, NULL);
	} else {
		shovelerLogInfo("Using windowed mode.");
		window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	}

	if(window == NULL) {
		shovelerLogError("Failed to create glfw window.");
		glfwTerminate();
		shovelerLogTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	if(!shovelerOpenGLCheckSuccess()) {
		glfwTerminate();
		shovelerLogTerminate();
		return EXIT_FAILURE;
	}

	glfwSwapInterval(vsync ? 1 : 0);

	if(!shovelerOpenGLCheckSuccess()) {
		glfwTerminate();
		shovelerLogTerminate();
		return EXIT_FAILURE;
	}

	shovelerLogInfo("Opened glfw window with name '%s', using OpenGL driver version '%s' by '%s'.", windowTitle, glGetString(GL_VERSION), glGetString(GL_VENDOR));

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		shovelerLogError("Failed to initialize GLEW with error code %d: %s.", err, glewGetErrorString(err));
		glfwTerminate();
		shovelerLogTerminate();
		return EXIT_FAILURE;
	}
	shovelerLogInfo("Using GLEW version '%s'.", glewGetString(GLEW_VERSION));

	GLenum error = glGetError();
	if(error = GL_INVALID_ENUM) {
		shovelerLogWarning("Initializing GLEW triggered GL_INVALID_ENUM, ignoring.");
	} else if(error != GL_NO_ERROR) {
		shovelerOpenGLHandleError(error);
		glfwTerminate();
		shovelerLogTerminate();
		return EXIT_FAILURE;
	}

	shovelerInputInit(window);
	shovelerInputAddKeyCallback(exitKeyHandler);

	shovelerSampleInit(window, width, height);

	double lastFrameTime = glfwGetTime();
	while(!glfwWindowShouldClose(window)) {
		double now = glfwGetTime();
		double dt = now - lastFrameTime;
		lastFrameTime = now;

		shovelerSampleRender(dt);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerSampleTerminate();

	shovelerInputTerminate();
	glfwTerminate();
	shovelerLogTerminate();
	return EXIT_SUCCESS;
}

static void exitKeyHandler(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		shovelerLogInfo("Escape key pressed, terminating.");
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

}
