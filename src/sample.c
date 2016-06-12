#include <glib.h>

#include "cameras/perspective.h"
#include "drawables/cube.h"
#include "model.h"
#include "opengl.h"
#include "sample.h"
#include "scene.h"
#include "shader_program.h"

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

static ShovelerMaterial *material;
static ShovelerDrawable *cube;
static ShovelerScene *scene;
static ShovelerCamerasPerspective *perspectiveCamera;

void shovelerSampleInit(int width, int height)
{
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
}

void shovelerSampleRender()
{
	shovelerSceneRender(scene, perspectiveCamera->camera);
}

void shovelerSampleTerminate()
{
	shovelerSceneFree(scene);
	shovelerCamerasPerspectiveFree(perspectiveCamera);
	shovelerDrawableFree(cube);
	shovelerMaterialFree(material);
}
