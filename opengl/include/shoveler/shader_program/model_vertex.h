#ifndef SHOVELER_SHADER_PROGRAM_MODEL_VERTEX_H
#define SHOVELER_SHADER_PROGRAM_MODEL_VERTEX_H

#include <shoveler/shader_program/model_vertex_projected.h>
#include <shoveler/shader_program/model_vertex_screenspace.h>
#include <stdbool.h> // bool

static inline GLuint shovelerShaderProgramModelVertexCreate(bool screenspace) {
  if (screenspace) {
    return shovelerShaderProgramModelVertexScreenspaceCreate();
  } else {
    return shovelerShaderProgramModelVertexProjectedCreate();
  }
}

#endif
