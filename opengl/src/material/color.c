#include "shoveler/material/depth.h"
#include "shoveler/shader_cache.h"
#include "shoveler/shader_program.h"
#include "shoveler/shader_program/model_vertex.h"
#include "shoveler/types.h"
#include "shoveler/uniform.h"
#include "shoveler/uniform_map.h"

static const char* fragmentShaderSource =
    "#version 400\n"
    ""
    "uniform vec3 cameraPosition;\n"
    ""
    "uniform vec3 lightColor;\n"
    "uniform float lightAmbientFactor;\n"
    "uniform float lightExponentialShadowFactor;\n"
    "uniform vec3 lightPosition;\n"
    "uniform bool isExponentialLiftedShadowMap;\n"
    "uniform sampler2D shadowMap;\n"
    ""
    "uniform vec4 color;\n"
    ""
    "in vec3 worldPosition;\n"
    "in vec3 worldNormal;\n"
    "in vec2 worldUv;\n"
    "in vec4 lightFrustumPosition4;\n"
    ""
    "out vec4 fragmentColor;\n"
    ""
    "bool isInLightCamera(vec3 lightScreenPosition)\n"
    "{\n"
    "	return lightScreenPosition.x >= 0.0 && lightScreenPosition.x <= 1.0 && "
    "lightScreenPosition.y >= 0.0 && lightScreenPosition.y <= 1.0;\n"
    "}\n"
    ""
    "float getExponentialShadowFactor(float shadowMapDepth, float fragmentDepth)\n"
    "{\n"
    "	if(isExponentialLiftedShadowMap) {\n"
    "		return clamp(shadowMapDepth * exp(-fragmentDepth * lightExponentialShadowFactor), 0.0, "
    "1.0);\n"
    "	} else {\n"
    "		return clamp(exp(shadowMapDepth * lightExponentialShadowFactor) * exp(-fragmentDepth * "
    "lightExponentialShadowFactor), 0.0, 1.0);\n"
    "	}\n"
    "}\n"
    ""
    "void main()\n"
    "{\n"
    "	vec3 lightFrustumPosition = lightFrustumPosition4.xyz / lightFrustumPosition4.w;\n"
    "	vec3 lightScreenPosition = 0.5 * (lightFrustumPosition + vec3(1.0, 1.0, 1.0));\n"
    "	float exponentialShadowFactor = 0.0;\n"
    "	if(isInLightCamera(lightScreenPosition)) {\n"
    "		float shadowMapDepth = texture2D(shadowMap, lightScreenPosition.xy).r;\n"
    "		float fragmentDepth = lightScreenPosition.z;\n"
    "		exponentialShadowFactor = getExponentialShadowFactor(shadowMapDepth, fragmentDepth);\n"
    "	}\n"
    ""
    "	vec3 lightDirection = normalize(worldPosition - lightPosition);\n"
    "	vec3 normal = normalize(worldNormal);\n"
    ""
    "	float diffuseFactor = clamp(dot(-lightDirection, normal), 0.0, 1.0);\n"
    ""
    "	vec3 cameraDirection = normalize(cameraPosition - worldPosition);\n"
    "	vec3 reflectedLight = -reflect(-lightDirection, normal);\n"
    "	float specularFactor = pow(clamp(dot(reflectedLight, cameraDirection), 0.0, 1.0), 250.0);\n"
    ""
    "	fragmentColor = vec4(clamp(lightAmbientFactor * color.rgb + exponentialShadowFactor * "
    "diffuseFactor * color.rgb * lightColor + exponentialShadowFactor * specularFactor * "
    "lightColor, 0.0, 1.0), color.a);\n"
    "}\n";

ShovelerMaterial* shovelerMaterialColorCreate(
    ShovelerShaderCache* shaderCache, bool screenspace, ShovelerVector4 color) {
  GLuint vertexShaderObject = shovelerShaderProgramModelVertexCreate(screenspace);
  GLuint fragmentShaderObject =
      shovelerShaderProgramCompileFromString(fragmentShaderSource, GL_FRAGMENT_SHADER);
  GLuint program = shovelerShaderProgramLink(vertexShaderObject, 0, fragmentShaderObject, true);

  ShovelerMaterial* material = shovelerMaterialCreate(shaderCache, screenspace, program);

  shovelerUniformMapInsert(material->uniforms, "color", shovelerUniformCreateVector4(color));

  return material;
}
