#include "opengl.h"

bool shovelerOpenGLCheckSuccessFull(const char *file, int line)
{
	GLenum error = glGetError();
	if(error == GL_NO_ERROR) {
		return true;
	}

	shovelerOpenGLHandleErrorFull(file, line, error);

	return false;
}

void shovelerOpenGLHandleErrorFull(const char *file, int line, GLenum error)
{
	switch(error) {
		case GL_INVALID_ENUM:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_INVALID_ENUM error: An unacceptable value is specified for an enumerated argument.");
		break;
		case GL_INVALID_VALUE:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_INVALID_VALUE error: A numeric argument is out of range.");
		break;
		case GL_INVALID_OPERATION:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_INVALID_OPERATION error: The specified operation is not allowed in the current state.");
		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_INVALID_FRAMEBUFFER_OPERATION error: The framebuffer object is not complete.");
		break;
		case GL_OUT_OF_MEMORY:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_OUT_OF_MEMORY error: There is not enough memory left to execute the command.");
		break;
		case GL_STACK_UNDERFLOW:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_STACK_UNDERFLOW error: An attempt has been made to perform an operation that would cause an internal stack to underflow.");
		break;
		case GL_STACK_OVERFLOW:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "OpenGL GL_STACK_OVERFLOW error: An attempt has been made to perform an operation that would cause an internal stack to overflow.");
		break;
		default:
			shovelerLogMessage(file, line, SHOVELER_LOG_LEVEL_ERROR, "unknown OpenGL error code %d", error);
		break;
	}
}
