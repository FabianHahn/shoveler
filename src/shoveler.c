#include <stdio.h> // stdout

#include <GLFW/glfw3.h>

#include "log.h"

int main(char *argv[], int argc)
{
	shovelerLogInit(SHOVELER_LOG_LEVEL_ALL, stdout);

	if(!glfwInit()) {
		shovelerLogError("failed to initialize glfw");
	}
	shovelerLogTrace("initialized glfw");
}
