#ifndef SHOVELER_ENTITY_ID_ALLOCATOR_H
#define SHOVELER_ENTITY_ID_ALLOCATOR_H

#include <glib.h>
#include <stdbool.h>

typedef struct {
	long long int nextFreshEntityId;
	/** set of (long long int) entity IDs */
	GHashTable* allocations;
	GArray* freeList;
} ShovelerEntityIdAllocator;

ShovelerEntityIdAllocator* shovelerCreateEntityIdAllocator();
long long int shovelerEntityIdAllocatorAllocate(ShovelerEntityIdAllocator* allocator);
bool shovelerEntityIdAllocatorDeallocate(ShovelerEntityIdAllocator* allocator, long long int entityId);
void shovelerEntityIdAllocatorFree(ShovelerEntityIdAllocator* allocator);

#endif
