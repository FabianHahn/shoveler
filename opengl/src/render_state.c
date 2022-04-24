#include "shoveler/render_state.h"

#include "shoveler/log.h"

void shovelerRenderStateReset(const ShovelerRenderState* renderState) {
  if (renderState->blend) {
    glEnable(GL_BLEND);
  } else {
    glDisable(GL_BLEND);
  }

  glBlendFunc(renderState->blendSourceFactor, renderState->blendDestinationFactor);

  if (renderState->depthTest) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  glDepthFunc(renderState->depthFunction);
  glDepthMask(renderState->depthMask);
}

void shovelerRenderStateSet(
    ShovelerRenderState* renderState, const ShovelerRenderState* targetRenderState) {
  if (targetRenderState->blend != renderState->blend) {
    if (targetRenderState->blend) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }

  if (targetRenderState->blendSourceFactor != renderState->blendSourceFactor ||
      targetRenderState->blendDestinationFactor != renderState->blendDestinationFactor) {
    glBlendFunc(targetRenderState->blendSourceFactor, targetRenderState->blendDestinationFactor);
  }

  if (targetRenderState->depthTest != renderState->depthTest) {
    if (targetRenderState->depthTest) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  if (targetRenderState->depthFunction != renderState->depthFunction) {
    glDepthFunc(targetRenderState->depthFunction);
  }

  if (targetRenderState->depthMask != renderState->depthMask) {
    glDepthMask(targetRenderState->depthMask);
  }

  *renderState = *targetRenderState;
}

void shovelerRenderStateSetVerbose(
    ShovelerRenderState* renderState, const ShovelerRenderState* targetRenderState) {
  if (targetRenderState->blend != renderState->blend) {
    shovelerLogInfo("%s blending.", targetRenderState->blend ? "Enabling" : "Disabling");
    if (targetRenderState->blend) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }

  if (targetRenderState->blendSourceFactor != renderState->blendSourceFactor ||
      targetRenderState->blendDestinationFactor != renderState->blendDestinationFactor) {
    shovelerLogInfo(
        "Changing blend function to source = %u, destination = %u.",
        targetRenderState->blendSourceFactor,
        targetRenderState->blendDestinationFactor);
    glBlendFunc(targetRenderState->blendSourceFactor, targetRenderState->blendDestinationFactor);
  }

  if (targetRenderState->depthTest != renderState->depthTest) {
    shovelerLogInfo("%s depth test.", targetRenderState->depthTest ? "Enabling" : "Disabling");
    if (targetRenderState->depthTest) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  if (targetRenderState->depthFunction != renderState->depthFunction) {
    shovelerLogInfo("Changing depth function to %u.", targetRenderState->depthFunction);
    glDepthFunc(targetRenderState->depthFunction);
  }

  if (targetRenderState->depthMask != renderState->depthMask) {
    shovelerLogInfo("%s depth mask.", targetRenderState->depthMask ? "Enabling" : "Disabling");
    glDepthMask(targetRenderState->depthMask);
  }

  *renderState = *targetRenderState;
}

void shovelerRenderStateEnableBlend(
    ShovelerRenderState* renderState, GLenum sourceFactor, GLenum destinationFactor) {
  renderState->blend = true;
  renderState->blendSourceFactor = sourceFactor;
  renderState->blendDestinationFactor = destinationFactor;

  glEnable(GL_BLEND);
  glBlendFunc(sourceFactor, destinationFactor);
}

void shovelerRenderStateDisableBlend(ShovelerRenderState* renderState) {
  renderState->blend = false;

  glDisable(GL_BLEND);
}

void shovelerRenderStateEnableDepthTest(ShovelerRenderState* renderState, GLenum depthFunction) {
  renderState->depthTest = true;
  renderState->depthFunction = depthFunction;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(depthFunction);
}

void shovelerRenderStateDisableDepthTest(ShovelerRenderState* renderState) {
  renderState->depthTest = false;

  glDisable(GL_DEPTH_TEST);
}

void shovelerRenderStateSetDepthMask(ShovelerRenderState* renderState, GLboolean enabled) {
  renderState->depthMask = enabled;

  glDepthMask(enabled);
}
