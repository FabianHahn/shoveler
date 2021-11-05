#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // memcpy

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/image/png.h>
#include <shoveler/resources/image_png.h>
#include <shoveler/component/drawable.h>
#include <shoveler/component/font.h>
#include <shoveler/component/font_atlas.h>
#include <shoveler/component/font_atlas_texture.h>
#include <shoveler/component/light.h>
#include <shoveler/component/material.h>
#include <shoveler/component/model.h>
#include <shoveler/component/position.h>
#include <shoveler/component/resource.h>
#include <shoveler/component/resources.h>
#include <shoveler/component/sampler.h>
#include <shoveler/component/text_texture_renderer.h>
#include <shoveler/component/texture.h>
#include <shoveler/component.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/file.h>
#include <shoveler/game_view.h>
#include <shoveler/global.h>
#include <shoveler/opengl.h>
#include <shoveler/projection.h>
#include <shoveler/resources.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

static double time = 0.0;

static GString *getImageData(ShovelerImage *image);
static void updateGame(ShovelerGame *game, double dt);
static void updateAuthoritativeViewComponent(ShovelerView *view, ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *userData);

int main(int argc, char *argv[])
{
	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	if(argc != 2) {
		shovelerLogError("Usage: %s [font file]", argv[0]);
		return EXIT_FAILURE;
	}

	const char *filename = argv[1];

	unsigned char *contents;
	size_t contentsSize;
	if(!shovelerFileRead(filename, &contents, &contentsSize)) {
		shovelerLogError("Failed to read font file.");
		return EXIT_FAILURE;
	}

	ShovelerGameWindowSettings windowSettings;
	windowSettings.windowTitle = "shoveler";
	windowSettings.fullscreen = false;
	windowSettings.vsync = true;
	windowSettings.samples = 4;
	windowSettings.windowedWidth = 640;
	windowSettings.windowedHeight = 480;

	ShovelerGameCameraSettings cameraSettings;
	cameraSettings.frame.position = shovelerVector3(0, 0, -5);
	cameraSettings.frame.direction = shovelerVector3(0, 0, 1);
	cameraSettings.frame.up = shovelerVector3(0, 1, 0);
	cameraSettings.projection.fieldOfViewY = 2.0f * SHOVELER_PI * 50.0f / 360.0f;
	cameraSettings.projection.aspectRatio = (float) windowSettings.windowedWidth / windowSettings.windowedHeight;
	cameraSettings.projection.nearClippingPlane = 0.01;
	cameraSettings.projection.farClippingPlane = 1000;

	ShovelerGameControllerSettings controllerSettings;
	controllerSettings.frame = cameraSettings.frame;
	controllerSettings.moveFactor = 2.0f;
	controllerSettings.tiltFactor = 0.0005f;
	controllerSettings.boundingBoxSize2 = 0.0f;
	controllerSettings.boundingBoxSize3 = 0.0f;

	ShovelerGame *game = shovelerGameCreate(updateGame, &windowSettings, &cameraSettings, &controllerSettings);
	if(game == NULL) {
		return EXIT_FAILURE;
	}
	ShovelerView *view = shovelerGameViewCreate(game, updateAuthoritativeViewComponent, NULL);

	game->controller->lockTiltX = true;
	game->controller->lockTiltY = true;

	ShovelerResources *resources = shovelerResourcesCreate(NULL, NULL);
	shovelerResourcesImagePngRegister(resources);
	shovelerViewSetTarget(view, shovelerComponentViewTargetIdResources, resources);

	ShovelerViewEntity *fontEntity = shovelerViewAddEntity(view, 1);
	ShovelerComponent *resourceComponent = shovelerViewEntityAddComponent(fontEntity, shovelerComponentTypeIdResource);
	shovelerComponentUpdateCanonicalConfigurationOptionBytes(resourceComponent, SHOVELER_COMPONENT_RESOURCE_OPTION_ID_BUFFER, contents, contentsSize);
	shovelerComponentActivate(resourceComponent);
	ShovelerComponent *fontComponent = shovelerViewEntityAddComponent(fontEntity, shovelerComponentTypeIdFont);
	shovelerComponentUpdateCanonicalConfigurationOptionString(fontComponent, SHOVELER_COMPONENT_FONT_OPTION_ID_NAME, "OpenSans");
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(fontComponent, SHOVELER_COMPONENT_FONT_OPTION_ID_RESOURCE, 1);
	shovelerComponentActivate(fontComponent);
	ShovelerComponent *fontAtlasComponent = shovelerViewEntityAddComponent(fontEntity, shovelerComponentTypeIdFontAtlas);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(fontAtlasComponent, SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT, 1);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(fontAtlasComponent, SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_FONT_SIZE, 48);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(fontAtlasComponent, SHOVELER_COMPONENT_FONT_ATLAS_OPTION_ID_PADDING, 1);
	shovelerComponentActivate(fontAtlasComponent);
	ShovelerComponent *fontAtlasTextureComponent = shovelerViewEntityAddComponent(fontEntity, shovelerComponentTypeIdFontAtlasTexture);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(fontAtlasTextureComponent, SHOVELER_COMPONENT_FONT_ATLAS_TEXTURE_OPTION_ID_FONT_ATLAS, 1);
	shovelerComponentActivate(fontAtlasTextureComponent);
	ShovelerComponent *textTextureRendererComponent = shovelerViewEntityAddComponent(fontEntity, shovelerComponentTypeIdTextTextureRenderer);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(textTextureRendererComponent, SHOVELER_COMPONENT_TEXT_TEXTURE_RENDERER_OPTION_ID_FONT_ATLAS_TEXTURE, 1);
	shovelerComponentActivate(textTextureRendererComponent);

	ShovelerViewEntity *textEntity = shovelerViewAddEntity(view, 2);
	ShovelerComponent *textureComponent = shovelerViewEntityAddComponent(textEntity, shovelerComponentTypeIdTexture);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(textureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TYPE, SHOVELER_COMPONENT_TEXTURE_TYPE_TEXT);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(textureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT_TEXTURE_RENDERER, 1);
	shovelerComponentUpdateCanonicalConfigurationOptionString(textureComponent, SHOVELER_COMPONENT_TEXTURE_OPTION_ID_TEXT, "shoveler");
	shovelerComponentActivate(textureComponent);
	ShovelerComponent *samplerComponent = shovelerViewEntityAddComponent(textEntity, shovelerComponentTypeIdSampler);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(samplerComponent, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_INTERPOLATE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(samplerComponent, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_USE_MIPMAPS, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(samplerComponent, SHOVELER_COMPONENT_SAMPLER_OPTION_ID_CLAMP, true);
	shovelerComponentActivate(samplerComponent);
	ShovelerComponent *drawableComponent = shovelerViewEntityAddComponent(textEntity, shovelerComponentTypeIdDrawable);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(drawableComponent, SHOVELER_COMPONENT_DRAWABLE_OPTION_ID_TYPE, SHOVELER_COMPONENT_DRAWABLE_TYPE_QUAD);
	shovelerComponentActivate(drawableComponent);
	ShovelerComponent *materialComponent = shovelerViewEntityAddComponent(textEntity, shovelerComponentTypeIdMaterial);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(materialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TYPE, SHOVELER_COMPONENT_MATERIAL_TYPE_TEXTURE);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(materialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_TYPE, SHOVELER_MATERIAL_TEXTURE_TYPE_ALPHA_MASK);
	shovelerComponentUpdateCanonicalConfigurationOptionVector4(materialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_COLOR, shovelerVector4(0.2f, 0.8f, 0.2f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(materialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(materialComponent, SHOVELER_COMPONENT_MATERIAL_OPTION_ID_TEXTURE_SAMPLER, 2);
	shovelerComponentActivate(materialComponent);
	ShovelerComponent *modelComponent = shovelerViewEntityAddComponent(textEntity, shovelerComponentTypeIdModel);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POSITION, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_DRAWABLE, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionEntityId(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_MATERIAL, 2);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_ROTATION, shovelerVector3(SHOVELER_PI, 0.0f, SHOVELER_PI));
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_SCALE, shovelerVector3(8.0f, 1.5f, 1.0f));
	shovelerComponentUpdateCanonicalConfigurationOptionBool(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_VISIBLE, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_EMITTER, true);
	shovelerComponentUpdateCanonicalConfigurationOptionBool(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_CASTS_SHADOW, false);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(modelComponent, SHOVELER_COMPONENT_MODEL_OPTION_ID_POLYGON_MODE, SHOVELER_COMPONENT_MODEL_POLYGON_MODE_FILL);
	shovelerComponentActivate(modelComponent);
	ShovelerComponent *positionComponent = shovelerViewEntityAddComponent(textEntity, shovelerComponentTypeIdPosition);
	shovelerComponentUpdateCanonicalConfigurationOptionInt(positionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_TYPE, SHOVELER_COMPONENT_POSITION_TYPE_ABSOLUTE);
	shovelerComponentUpdateCanonicalConfigurationOptionVector3(positionComponent, SHOVELER_COMPONENT_POSITION_OPTION_ID_COORDINATES, shovelerVector3(0.0f, 0.0f, 10.0f));
	shovelerComponentActivate(positionComponent);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	free(contents);
	shovelerGameFree(game);
	shovelerResourcesFree(resources);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static void updateGame(ShovelerGame *game, double dt)
{
	shovelerCameraUpdateView(game->camera);
}

static void updateAuthoritativeViewComponent(ShovelerView *view, ShovelerComponent *component, const ShovelerComponentTypeConfigurationOption *configurationOption, const ShovelerComponentConfigurationValue *value, void *userData)
{
	GString *printedValue = shovelerComponentConfigurationValuePrint(value);

	shovelerLogInfo("Updating configuration option '%s' of component '%s' on entity %lld: %s", configurationOption->name, component->type->id, component->entityId, printedValue->str);

	g_string_free(printedValue, true);
}
