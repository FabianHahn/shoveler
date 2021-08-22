#include "fakeglib/GArray.h"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#define MIN_ARRAY_SIZE 16

struct GRealArray {
	void Grow();
	void GrowBy(guint length);
	void GrowToSize(guint size);
	guint AllocatedSpaces() const;
	guint SpacesLeft() const;
	gpointer Element(guint index);
	void ClearElement(guint index);
	void ZeroTerminate();

	gchar *data;
	guint length;
	guint allocated;
	guint elementSize;
	GDestroyNotify clearFunction;
	std::atomic<int> refCount;
	bool isZeroTerminated;
	bool isCleared;
};

void GRealArray::Grow()
{
	gchar *newData = reinterpret_cast<gchar *>(malloc(2 * allocated * elementSize));
	memcpy(newData, data, allocated * elementSize);
	free(data);
	data = newData;

	if (isCleared) {
		memset(&data[allocated], 0, allocated * elementSize);
	}

	allocated *= 2;
}

void GRealArray::GrowBy(guint length)
{
	while (length > SpacesLeft()) {
		Grow();
	}
}

void GRealArray::GrowToSize(guint size)
{
	if (size <= length) {
		return;
	}

	GrowBy(size - length);
}

guint GRealArray::AllocatedSpaces() const
{
	return allocated - (isZeroTerminated ? 1 : 0);
}

guint GRealArray::SpacesLeft() const
{
	return AllocatedSpaces() - length;
}

gpointer GRealArray::Element(guint index)
{
	assert(index < allocated);
	return &data[index * elementSize];
}

void GRealArray::ClearElement(guint index)
{
	if (clearFunction != NULL) {
		clearFunction(Element(index));
	}
}

void GRealArray::ZeroTerminate()
{
	if (isZeroTerminated || isCleared) {
		memset(Element(length), 0, elementSize);
	}
}

FAKEGLIB_API GArray *g_array_new(gboolean isZeroTerminated, gboolean isCleared, guint elementSize)
{
	return g_array_sized_new(isZeroTerminated, isCleared, elementSize, /* reservedSize */ 0);
}

FAKEGLIB_API gpointer g_array_steal(GArray *array, gsize *length)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	gpointer stolenData = realArray->data;
	*length = realArray->length;

	realArray->data = reinterpret_cast<gchar *>(malloc(MIN_ARRAY_SIZE * realArray->elementSize));
	realArray->length = 0;
	realArray->allocated = MIN_ARRAY_SIZE;

	if (realArray->isCleared) {
		memset(realArray->data, 0, realArray->allocated * realArray->elementSize);
	} else if (realArray->isZeroTerminated) {
		memset(realArray->data, 0, realArray->elementSize);
	}

	return stolenData;
}

FAKEGLIB_API GArray *g_array_sized_new(gboolean isZeroTerminated, gboolean isCleared, guint elementSize, guint reservedSize)
{
	guint allocated = reservedSize;
	if (allocated < MIN_ARRAY_SIZE) {
		allocated = MIN_ARRAY_SIZE;
	}

	GRealArray *realArray = new GRealArray;
	realArray->data = reinterpret_cast<gchar *>(malloc(allocated * elementSize));
	realArray->length = 0;
	realArray->allocated = allocated;
	realArray->elementSize = elementSize;
	realArray->clearFunction = NULL;
	realArray->refCount = 1;
	realArray->isZeroTerminated = isZeroTerminated;
	realArray->isCleared = isCleared;

	if (realArray->isCleared) {
		memset(realArray->data, 0, realArray->allocated * realArray->elementSize);
	} else {
		realArray->ZeroTerminate();
	}

	return reinterpret_cast<GArray *>(realArray);
}

FAKEGLIB_API GArray *g_array_copy(GArray *array)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	GRealArray *realArrayCopy = new GRealArray;
	realArrayCopy->data = reinterpret_cast<gchar *>(malloc(realArray->allocated * realArray->elementSize));
	memcpy(realArrayCopy->data, realArray->data, realArray->allocated * realArray->elementSize);
	realArrayCopy->length = realArray->length;
	realArrayCopy->allocated = realArray->allocated;
	realArrayCopy->elementSize = realArray->elementSize;
	realArrayCopy->clearFunction = realArray->clearFunction;
	realArrayCopy->refCount = 1;
	realArrayCopy->isZeroTerminated = realArray->isZeroTerminated;
	realArrayCopy->isCleared = realArray->isCleared;

	return reinterpret_cast<GArray *>(realArrayCopy);
}

FAKEGLIB_API GArray *g_array_ref(GArray *array)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	realArray->refCount++;
	return array;
}

FAKEGLIB_API void g_array_unref(GArray *array)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	if (std::atomic_fetch_sub(&realArray->refCount, 1) == 1) {
		g_array_free(array, true);
	}
}

FAKEGLIB_API guint g_array_get_element_size(GArray *array)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	return realArray->elementSize;
}

FAKEGLIB_API GArray *g_array_append_vals(GArray *array, gconstpointer data, guint length)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	if (length == 0) {
		return array;
	}

	realArray->GrowBy(length);

	memcpy(realArray->Element(realArray->length), data, length * realArray->elementSize);
	realArray->length += length;

	if (!realArray->isCleared) {
		realArray->ZeroTerminate();
	}

	return array;
}

FAKEGLIB_API GArray *g_array_prepend_vals(GArray *array, gconstpointer data, guint length)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	if (length == 0) {
		return array;
	}

	realArray->GrowBy(length);

	memmove(realArray->Element(length), realArray->data, realArray->length * realArray->elementSize);
	memcpy(realArray->data, data, length * realArray->elementSize);
	realArray->length += length;

	if (!realArray->isCleared) {
		realArray->ZeroTerminate();
	}

	return array;
}

FAKEGLIB_API GArray *g_array_insert_vals(GArray *array, guint index, gconstpointer data, guint length)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	if (length == 0) {
		return array;
	}

	guint numExtraElements = 0;
	guint newLength = realArray->length + length;
	if (index > realArray->length) {
		numExtraElements = index - realArray->length;
		newLength += numExtraElements;
	}

	realArray->GrowToSize(newLength);

	if (numExtraElements > 0 && realArray->isCleared) {
		memset(realArray->Element(realArray->length), 0, numExtraElements * realArray->elementSize);
		realArray->length += numExtraElements;
	}

	if (index < realArray->length) {
		memmove(realArray->Element(index + length), realArray->Element(index), length * realArray->elementSize);
	}

	memcpy(realArray->Element(index), data, length * realArray->elementSize);
	realArray->length += length;

	if (!realArray->isCleared) {
		realArray->ZeroTerminate();
	}

	return array;
}

FAKEGLIB_API GArray *g_array_remove_index(GArray *array, guint index)
{
	return g_array_remove_range(array, index, 1);
}

FAKEGLIB_API GArray *g_array_remove_index_fast(GArray *array, guint index)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	assert(index < realArray->length);

	realArray->ClearElement(index);

	memcpy(realArray->Element(index), realArray->Element(realArray->length - 1), realArray->elementSize);
	realArray->length--;
	realArray->ZeroTerminate();

	return array;
}

FAKEGLIB_API GArray *g_array_remove_range(GArray *array, guint index, guint length)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	assert(index + length <= realArray->length);

	if (realArray->clearFunction != NULL) {
		for (guint i = index; i < index + length; i++) {
			realArray->ClearElement(i);
		}
	}

	guint numElementsAfter = realArray->length - index - length;
	if (numElementsAfter == 0) {
		realArray->length -= length;
		return array;
	}

	memcpy(realArray->Element(index), realArray->Element(index + length), numElementsAfter * realArray->elementSize);
	realArray->length -= length;

	if (realArray->isCleared) {
		memset(realArray->Element(realArray->length), 0, length * realArray->elementSize);
	} else {
		realArray->ZeroTerminate();
	}

	return array;
}

FAKEGLIB_API GArray *g_array_set_size(GArray *array, guint length)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	if (length == realArray->length) {
		return array;
	}

	if (length > realArray->length) {
		realArray->GrowToSize(length);

		guint numInsertedElements = length - realArray->length;

		if (realArray->isCleared) {
			memset(realArray->Element(realArray->length), 0, numInsertedElements * realArray->elementSize);
		} else {
			realArray->ZeroTerminate();
		}
	} else {
		g_array_remove_range(array, length, realArray->length - length);
	}

	realArray->length = length;
	return array;
}

FAKEGLIB_API void g_array_set_clear_func(GArray *array, GDestroyNotify clearFunction)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	realArray->clearFunction = clearFunction;
}

FAKEGLIB_API gchar *g_array_free(GArray *array, gboolean freeSegment)
{
	GRealArray *realArray = reinterpret_cast<GRealArray *>(array);

	if (realArray == NULL) {
		return NULL;
	}

	gchar *data = realArray->data;
	if (!freeSegment) {
		delete realArray;
		return data;
	}

	if (realArray->clearFunction != NULL) {
		for (guint i = 0; i < realArray->length; i++) {
			realArray->ClearElement(i);
		}
	}

	free(data);
	delete realArray;
	return NULL;

}
