#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#include <glib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shoveler/camera/perspective.h>
#include <shoveler/image/png.h>
#include <shoveler/resources/image_png.h>
#include <shoveler/view/drawable.h>
#include <shoveler/view/light.h>
#include <shoveler/view/material.h>
#include <shoveler/view/model.h>
#include <shoveler/view/position.h>
#include <shoveler/view/resources.h>
#include <shoveler/view/texture.h>
#include <shoveler/view/tilemap.h>
#include <shoveler/view/tileset.h>
#include <shoveler/constants.h>
#include <shoveler/controller.h>
#include <shoveler/file.h>
#include <shoveler/global.h>
#include <shoveler/opengl.h>
#include <shoveler/resources.h>
#include <shoveler/scene.h>
#include <shoveler/types.h>
#include <shoveler/view.h>

static double time = 0.0;
static ShovelerView *view = NULL;

static GString *getImageData(ShovelerImage *image);
static void updateGame(ShovelerGame *game, double dt);

int main(int argc, char *argv[])
{
	const char *windowTitle = "shoveler";
	bool fullscreen = false;
	bool vsync = true;
	int samples = 4;
	int width = 640;
	int height = 480;

	shovelerLogInit("shoveler/", SHOVELER_LOG_LEVEL_INFO_UP, stdout);
	shovelerGlobalInit();

	ShovelerGame *game = shovelerGameCreate(windowTitle, width, height, samples, fullscreen, vsync);
	if(game == NULL) {
		return EXIT_FAILURE;
	}

	game->camera = shovelerCameraPerspectiveCreate(shovelerVector3(0.0, 0.0, -5.0), shovelerVector3(0.0, 0.0, 1.0), shovelerVector3(0.0, 1.0, 0.0), 2.0f * SHOVELER_PI * 50.0f / 360.0f, (float) width / height, 0.01, 1000);
	game->scene = shovelerSceneCreate();
	game->update = updateGame;

	ShovelerController *controller = shovelerControllerCreate(game, shovelerVector3(0.0, 0.0, -5.0), shovelerVector3(0.0, 0.0, 1.0), shovelerVector3(0.0, 1.0, 0.0), 2.0f, 0.0005f);
	shovelerCameraPerspectiveAttachController(game->camera, controller);

	ShovelerResources *resources = shovelerResourcesCreate(NULL, NULL);
	shovelerResourcesImagePngRegister(resources);

	view = shovelerViewCreate();
	shovelerViewSetTarget(view, "controller", controller);
	shovelerViewSetResources(view, resources);
	shovelerViewSetTarget(view, "scene", game->scene);

	ShovelerViewDrawableConfiguration cubeDrawableConfiguration;
	cubeDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_CUBE;
	ShovelerViewEntity *cubeDrawableEntity = shovelerViewAddEntity(view, 1);
	shovelerViewEntityAddDrawable(cubeDrawableEntity, cubeDrawableConfiguration);

	ShovelerViewMaterialConfiguration grayColorMaterialConfiguration;
	grayColorMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_COLOR;
	grayColorMaterialConfiguration.color = shovelerVector3(0.7, 0.7, 0.7);
	ShovelerViewEntity *grayColorMaterialEntity = shovelerViewAddEntity(view, 2);
	shovelerViewEntityAddMaterial(grayColorMaterialEntity, grayColorMaterialConfiguration);

	ShovelerViewModelConfiguration cubeModelConfiguration;
	cubeModelConfiguration.drawableEntityId = 1;
	cubeModelConfiguration.materialEntityId = 7;
	cubeModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	cubeModelConfiguration.scale = shovelerVector3(1.0, 1.0, 1.0);
	cubeModelConfiguration.visible = true;
	cubeModelConfiguration.emitter = false;
	cubeModelConfiguration.screenspace = false;
	cubeModelConfiguration.castsShadow = true;
	cubeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *cubeEntity = shovelerViewAddEntity(view, 3);
	shovelerViewEntityAddModel(cubeEntity, cubeModelConfiguration);
	shovelerViewEntityAddPosition(cubeEntity, 0.0, 0.0, 5.0);

	ShovelerViewDrawableConfiguration quadDrawableConfiguration;
	quadDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_QUAD;
	ShovelerViewModelConfiguration planeModelConfiguration;
	planeModelConfiguration.drawableEntityId = 4;
	planeModelConfiguration.materialEntityId = 2;
	planeModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, 0.0);
	planeModelConfiguration.scale = shovelerVector3(10.0, 10.0, 1.0);
	planeModelConfiguration.visible = true;
	planeModelConfiguration.emitter = false;
	planeModelConfiguration.screenspace = false;
	planeModelConfiguration.castsShadow = true;
	planeModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *planeEntity = shovelerViewAddEntity(view, 4);
	shovelerViewEntityAddDrawable(planeEntity, quadDrawableConfiguration);
	shovelerViewEntityAddModel(planeEntity, planeModelConfiguration);
	shovelerViewEntityAddPosition(planeEntity, 0.0, 0.0, 10.0);

	ShovelerViewDrawableConfiguration pointDrawableConfiguration;
	pointDrawableConfiguration.type = SHOVELER_VIEW_DRAWABLE_TYPE_POINT;
	ShovelerViewMaterialConfiguration whiteParticleMaterialConfiguration;
	whiteParticleMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_PARTICLE;
	whiteParticleMaterialConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewModelConfiguration lightModelConfiguration;
	lightModelConfiguration.drawableEntityId = 5;
	lightModelConfiguration.materialEntityId = 5;
	lightModelConfiguration.rotation = shovelerVector3(0.0, 0.0, 0.0);
	lightModelConfiguration.scale = shovelerVector3(0.5, 0.5, 0.5);
	lightModelConfiguration.visible = true;
	lightModelConfiguration.emitter = true;
	lightModelConfiguration.screenspace = false;
	lightModelConfiguration.castsShadow = false;
	lightModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewLightConfiguration lightConfiguration;
	lightConfiguration.type = SHOVELER_VIEW_LIGHT_TYPE_POINT;
	lightConfiguration.width = 512;
	lightConfiguration.height = 512;
	lightConfiguration.samples = 1;
	lightConfiguration.ambientFactor = 0.0;
	lightConfiguration.exponentialFactor = 80.0;
	lightConfiguration.color = shovelerVector3(1.0, 1.0, 1.0);
	ShovelerViewEntity *lightEntity = shovelerViewAddEntity(view, 5);
	shovelerViewEntityAddDrawable(lightEntity, pointDrawableConfiguration);
	shovelerViewEntityAddMaterial(lightEntity, whiteParticleMaterialConfiguration);
	shovelerViewEntityAddModel(lightEntity, lightModelConfiguration);
	shovelerViewEntityAddLight(lightEntity, lightConfiguration);
	shovelerViewEntityAddPosition(lightEntity, 0.0, 2.0, 0.0);

	ShovelerImage *image = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(image);
	shovelerImageGet(image, 0, 0, 0) = 255;
	shovelerImageGet(image, 0, 1, 1) = 255;
	shovelerImageGet(image, 1, 0, 2) = 255;
	shovelerImageGet(image, 1, 1, 1) = 255;
	shovelerImageGet(image, 1, 1, 2) = 255;
	GString *imageData = getImageData(image);
	shovelerImageFree(image);
	ShovelerViewResourceConfiguration imageResourceConfiguration;
	imageResourceConfiguration.typeId = "image/png";
	imageResourceConfiguration.buffer = (unsigned char *) imageData->str;
	imageResourceConfiguration.bufferSize = imageData->len;
	ShovelerViewTextureConfiguration textureConfiguration;
	textureConfiguration.imageResourceEntityId = 6;
	textureConfiguration.interpolate = true;
	textureConfiguration.clamp = true;
	ShovelerViewEntity *resourceEntity = shovelerViewAddEntity(view, 6);
	shovelerViewEntityAddResource(resourceEntity, imageResourceConfiguration);
	shovelerViewEntityAddTexture(resourceEntity, textureConfiguration);
	g_string_free(imageData, true);

	ShovelerViewMaterialConfiguration textureMaterialConfiguration;
	textureMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_TEXTURE;
	textureMaterialConfiguration.textureEntityId = 6;
	ShovelerViewEntity *textureMaterialEntity = shovelerViewAddEntity(view, 7);
	shovelerViewEntityAddMaterial(textureMaterialEntity, textureMaterialConfiguration);

	ShovelerImage *layerImage = shovelerImageCreate(2, 2, 3);
	shovelerImageClear(layerImage);
	shovelerImageGet(layerImage, 0, 0, 0) = 0;
	shovelerImageGet(layerImage, 0, 0, 1) = 0;
	shovelerImageGet(layerImage, 0, 0, 2) = 1; // red
	shovelerImageGet(layerImage, 0, 1, 0) = 0;
	shovelerImageGet(layerImage, 0, 1, 1) = 0;
	shovelerImageGet(layerImage, 0, 1, 2) = 1; // red
	shovelerImageGet(layerImage, 1, 0, 0) = 0;
	shovelerImageGet(layerImage, 1, 0, 1) = 1;
	shovelerImageGet(layerImage, 1, 0, 2) = 1; // green
	shovelerImageGet(layerImage, 1, 1, 0) = 0;
	shovelerImageGet(layerImage, 1, 1, 1) = 0;
	shovelerImageGet(layerImage, 1, 1, 2) = 2; // full tileset
	GString *layerImageData = getImageData(layerImage);
	shovelerImageFree(layerImage);
	ShovelerViewResourceConfiguration layerImageResourceConfiguration;
	layerImageResourceConfiguration.typeId = "image/png";
	layerImageResourceConfiguration.buffer = (unsigned char *) layerImageData->str;
	layerImageResourceConfiguration.bufferSize = layerImageData->len;
	ShovelerViewEntity *layerResourceEntity = shovelerViewAddEntity(view, 8);
	shovelerViewEntityAddResource(layerResourceEntity, layerImageResourceConfiguration);
	g_string_free(layerImageData, true);

	ShovelerImage *layer2Image = shovelerImageCreate(3, 3, 3);
	shovelerImageClear(layer2Image);
	shovelerImageGet(layer2Image, 0, 0, 0) = 0;
	shovelerImageGet(layer2Image, 0, 0, 1) = 0;
	shovelerImageGet(layer2Image, 0, 0, 2) = 2; // full tileset
	GString *layer2ImageData = getImageData(layer2Image);
	shovelerImageFree(layer2Image);
	ShovelerViewResourceConfiguration layer2ImageResourceConfiguration;
	layer2ImageResourceConfiguration.typeId = "image/png";
	layer2ImageResourceConfiguration.buffer = (unsigned char *) layer2ImageData->str;
	layer2ImageResourceConfiguration.bufferSize = layer2ImageData->len;
	ShovelerViewEntity *layer2ResourceEntity = shovelerViewAddEntity(view, 9);
	shovelerViewEntityAddResource(layer2ResourceEntity, layer2ImageResourceConfiguration);
	g_string_free(layer2ImageData, true);

	ShovelerViewTilesetConfiguration tilesetConfiguration;
	tilesetConfiguration.textureEntityId = 6;
	tilesetConfiguration.columns = 2;
	tilesetConfiguration.rows = 2;
	ShovelerViewEntity *tilesetMaterialEntity = shovelerViewAddEntity(view, 10);
	shovelerViewEntityAddTileset(tilesetMaterialEntity, tilesetConfiguration);

	ShovelerViewTilesetConfiguration tileset2Configuration;
	tileset2Configuration.textureEntityId = 6;
	tileset2Configuration.columns = 1;
	tileset2Configuration.rows = 1;
	ShovelerViewEntity *tileset2MaterialEntity = shovelerViewAddEntity(view, 11);
	shovelerViewEntityAddTileset(tileset2MaterialEntity, tileset2Configuration);

	ShovelerViewTilemapConfiguration tilemapConfiguration;
	tilemapConfiguration.numLayers = 2;
	tilemapConfiguration.layerImageResourceEntityIds = (long long int[]){8, 9};
	tilemapConfiguration.numTilesets = 2;
	tilemapConfiguration.tilesetEntityIds = (long long int[]){10, 11};
	ShovelerViewMaterialConfiguration tilemapMaterialConfiguration;
	tilemapMaterialConfiguration.type = SHOVELER_VIEW_MATERIAL_TYPE_TILEMAP;
	tilemapMaterialConfiguration.tilemapEntityId = 12;
	ShovelerViewEntity *tilemapMaterialEntity = shovelerViewAddEntity(view, 12);
	shovelerViewEntityAddTilemap(tilemapMaterialEntity, tilemapConfiguration);
	shovelerViewEntityAddMaterial(tilemapMaterialEntity, tilemapMaterialConfiguration);

	ShovelerViewModelConfiguration tilemapModelConfiguration;
	tilemapModelConfiguration.drawableEntityId = 4;
	tilemapModelConfiguration.materialEntityId = 12;
	tilemapModelConfiguration.rotation = shovelerVector3(SHOVELER_PI, 0.0, SHOVELER_PI);
	tilemapModelConfiguration.scale = shovelerVector3(0.5, 0.5, 1.0);
	tilemapModelConfiguration.visible = true;
	tilemapModelConfiguration.emitter = false;
	tilemapModelConfiguration.screenspace = false;
	tilemapModelConfiguration.castsShadow = true;
	tilemapModelConfiguration.polygonMode = GL_FILL;
	ShovelerViewEntity *tilemapModelEntity = shovelerViewAddEntity(view, 13);
	shovelerViewEntityAddModel(tilemapModelEntity, tilemapModelConfiguration);
	shovelerViewEntityAddPosition(tilemapModelEntity, 5.0, 5.0, 7.5);

	shovelerOpenGLCheckSuccess();

	while(shovelerGameIsRunning(game)) {
		shovelerGameRenderFrame(game);
	}
	shovelerLogInfo("Exiting main loop, goodbye.");

	shovelerViewFree(view);
	shovelerResourcesFree(resources);
	shovelerCameraPerspectiveDetachController(game->camera);
	shovelerControllerFree(controller);
	shovelerSceneFree(game->scene);
	shovelerCameraFree(game->camera);
	shovelerGameFree(game);
	shovelerGlobalUninit();
	shovelerLogTerminate();

	return EXIT_SUCCESS;
}

static GString *getImageData(ShovelerImage *image)
{
	const char *tempImageFilename = "temp.png";
	shovelerImagePngWriteFile(image, tempImageFilename);

	unsigned char *contents;
	size_t contentsSize;
	shovelerFileRead(tempImageFilename, &contents, &contentsSize);

	GString *data = g_string_new("");
	g_string_append_len(data, (gchar *) contents, contentsSize);
	free(contents);

	return data;
}

static void updateGame(ShovelerGame *game, double dt)
{
	shovelerControllerUpdate(shovelerCameraPerspectiveGetController(game->camera), dt);
	shovelerCameraUpdateView(game->camera);

	time += dt;
	ShovelerViewEntity *lightEntity = shovelerViewGetEntity(view, 5);
	shovelerViewEntityUpdatePosition(lightEntity, 2.0 * sin(time), 2.0 * cos(time), 0.0);
}
