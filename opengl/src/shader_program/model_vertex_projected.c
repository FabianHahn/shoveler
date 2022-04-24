#include "shoveler/shader_program/model_vertex_projected.h"

#include "shoveler/shader_program.h"

static const char* vertexShaderSource =
    "#version 400\n"
    ""
    "uniform mat4 model;\n"
    "uniform mat4 modelNormal;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 lightView;\n"
    "uniform mat4 lightProjection;\n"
    ""
    "in vec3 position;\n"
    "in vec3 normal;\n"
    "in vec2 uv;\n"
    ""
    "out vec3 worldPosition;"
    "out vec3 worldNormal;"
    "out vec2 worldUv;"
    "out vec4 lightFrustumPosition4;"
    ""
    "void main()\n"
    "{\n"
    "	vec4 worldPosition4 = model * vec4(position, 1.0);\n"
    "	vec4 worldNormal4 = modelNormal * vec4(normal, 1.0);\n"
    "	worldPosition = worldPosition4.xyz / worldPosition4.w;\n"
    "	worldNormal = worldNormal4.xyz / worldNormal4.w;\n"
    "	worldUv = uv;\n"
    ""
    "	lightFrustumPosition4 = lightProjection * lightView * worldPosition4;\n"
    ""
    "	gl_Position = projection * view * worldPosition4;\n"
    "}\n";

GLuint shovelerShaderProgramModelVertexProjectedCreate() {
  return shovelerShaderProgramCompileFromString(vertexShaderSource, GL_VERTEX_SHADER);
}
