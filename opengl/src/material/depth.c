#include "shoveler/material/depth.h"

#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/shader_program/model_vertex.h"

static const char* fragmentShaderSource =
    "#version 400\n"
    "\n"
    "out vec4 fragmentDepth;\n"
    "\n"
    "void main()\n"
    "{\n"
    "	fragmentDepth = vec4(gl_FragCoord.z);\n"
    "}\n";

ShovelerMaterial* shovelerMaterialDepthCreate(ShovelerShaderCache* shaderCache, bool screenspace) {
  GLuint vertexShaderObject = shovelerShaderProgramModelVertexCreate(screenspace);
  GLuint fragmentShaderObject =
      shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);
  return shovelerMaterialCreate(shaderCache, screenspace, program);
}
