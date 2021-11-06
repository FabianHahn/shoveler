#ifndef SHOVELER_INPUT_H
#define SHOVELER_INPUT_H

#include <stdbool.h> // bool

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct ShovelerGameStruct; // forward delcaration
struct ShovelerInputStruct; // forward declaration

typedef void (ShovelerInputKeyCallbackFunction)(struct ShovelerInputStruct *input, int key, int scancode, int action, int mods, void *userData);
typedef void (ShovelerInputMouseButtonCallbackFunction)(struct ShovelerInputStruct *input, int button, int action, int mods, void *userData);
typedef void (ShovelerInputCursorPositionCallbackFunction)(struct ShovelerInputStruct *input, double xpos, double ypos, void *userData);
typedef void (ShovelerInputScrollCallbackFunction)(struct ShovelerInputStruct *input, double xoffset, double yoffset, void *userData);
typedef void (ShovelerInputWindowSizeCallbackFunction)(struct ShovelerInputStruct *input, unsigned int width, unsigned int height, void *userData);

typedef struct ShovelerInputKeyCallbackStruct {
	ShovelerInputKeyCallbackFunction *function;
	void *userData;
} ShovelerInputKeyCallback;

typedef struct ShovelerInputMouseButtonCallbackStruct {
	ShovelerInputMouseButtonCallbackFunction *function;
	void *userData;
} ShovelerInputMouseButtonCallback;

typedef struct ShovelerInputCursorPositionCallbackStruct {
	ShovelerInputCursorPositionCallbackFunction *function;
	void *userData;
} ShovelerInputCursorPositionCallback;

typedef struct ShovelerInputScrollCallbackStruct {
	ShovelerInputScrollCallbackFunction *function;
	void *userData;
} ShovelerInputScrollCallback;

typedef struct ShovelerInputWindowSizeCallbackStruct {
	ShovelerInputWindowSizeCallbackFunction *function;
	void *userData;
} ShovelerInputWindowSizeCallback;

typedef struct ShovelerInputStruct {
	struct ShovelerGameStruct *game;
	/** set of (ShovelerInputKeyCallback *) */
	GHashTable *keyCallbacks;
	/** set of (ShovelerInputMouseButtonCallback *) */
	GHashTable *mouseButtonCallbacks;
	/** set of (ShovelerInputCursorPositionCallback *) */
	GHashTable *cursorPositionCallbacks;
	/** set of (ShovelerInputScrollCallback *) */
	GHashTable *scrollCallbacks;
	/** set of (ShovelerInputWindowSizeCallback *) */
	GHashTable *windowSizeCallbacks;
} ShovelerInput;

ShovelerInput *shovelerInputCreate(struct ShovelerGameStruct *game);
ShovelerInputKeyCallback *shovelerInputAddKeyCallback(ShovelerInput *input, ShovelerInputKeyCallbackFunction *callbackFunction, void *userData);
ShovelerInputMouseButtonCallback *shovelerInputAddMouseButtonCallback(ShovelerInput *input, ShovelerInputMouseButtonCallbackFunction *callbackFunction, void *userData);
ShovelerInputCursorPositionCallback *shovelerInputAddCursorPosCallback(ShovelerInput *input, ShovelerInputCursorPositionCallbackFunction *callbackFunction, void *userData);
ShovelerInputScrollCallback *shovelerInputAddScrollCallback(ShovelerInput *input, ShovelerInputScrollCallbackFunction *callbackFunction, void *userData);
ShovelerInputWindowSizeCallback *shovelerInputAddWindowSizeCallback(ShovelerInput *input, ShovelerInputWindowSizeCallbackFunction *callbackFunction, void *userData);
bool shovelerInputRemoveKeyCallback(ShovelerInput *input, ShovelerInputKeyCallback *callback);
bool shovelerInputRemoveMouseButtonCallback(ShovelerInput *input, ShovelerInputMouseButtonCallback *callback);
bool shovelerInputRemoveCursorPositionCallback(ShovelerInput *input, ShovelerInputCursorPositionCallback *callback);
bool shovelerInputRemoveScrollCallback(ShovelerInput *input, ShovelerInputScrollCallback *callback);
bool shovelerInputRemoveWindowSizeCallback(ShovelerInput *input, ShovelerInputWindowSizeCallback *callback);
void shovelerInputFree(ShovelerInput *input);

#endif
