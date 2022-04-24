#include "shoveler/resources/image_png.h"

#include "shoveler/image/png.h"
#include "shoveler/log.h"

static void* loadPng(
    ShovelerResourcesTypeLoader* typeLoader, const unsigned char* buffer, int bufferSize);
static void freePng(ShovelerResourcesTypeLoader* typeLoader, void* resourceData);
static void freeTypeLoader(ShovelerResourcesTypeLoader* typeLoader);
static ShovelerImage* createDefaultImage();

bool shovelerResourcesImagePngRegister(ShovelerResources* resources) {
  ShovelerResourcesTypeLoader imagePngTypeLoader;
  imagePngTypeLoader.typeId = shovelerResourcesImagePngTypeId;
  imagePngTypeLoader.defaultResourceData = createDefaultImage();
  imagePngTypeLoader.data = NULL;
  imagePngTypeLoader.load = loadPng;
  imagePngTypeLoader.freeResourceData = freePng;
  imagePngTypeLoader.free = freeTypeLoader;
  return shovelerResourcesRegisterTypeLoader(resources, imagePngTypeLoader);
}

static void* loadPng(
    ShovelerResourcesTypeLoader* typeLoader, const unsigned char* buffer, int bufferSize) {
  return shovelerImagePngReadBuffer(buffer, bufferSize);
}

static void freePng(ShovelerResourcesTypeLoader* typeLoader, void* resourceData) {
  ShovelerImage* pngImage = (ShovelerImage*) resourceData;
  shovelerImageFree(resourceData);
}

static void freeTypeLoader(ShovelerResourcesTypeLoader* typeLoader) {
  // nothing to do here
}

static ShovelerImage* createDefaultImage() {
  ShovelerImage* defaultImage = shovelerImageCreate(2, 2, 3);
  shovelerImageClear(defaultImage);
  shovelerImageGet(defaultImage, 0, 0, 0) = 255; // red
  shovelerImageGet(defaultImage, 0, 1, 1) = 255; // green
  shovelerImageGet(defaultImage, 1, 0, 2) = 255; // blue
  shovelerImageGet(defaultImage, 1, 1, 0) = 255;
  shovelerImageGet(defaultImage, 1, 1, 1) = 255;
  shovelerImageGet(defaultImage, 1, 1, 2) = 255; // white
  return defaultImage;
}
