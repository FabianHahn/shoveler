#include "shoveler/entity_id_allocator.h"

#include <stdlib.h>

ShovelerEntityIdAllocator* shovelerCreateEntityIdAllocator()
{
	ShovelerEntityIdAllocator* allocator = malloc(sizeof(ShovelerEntityIdAllocator));
	allocator->nextFreshEntityId = 1;
	allocator->allocations = g_hash_table_new_full(g_int64_hash, g_int64_equal, free, NULL);
	allocator->freeList = g_array_new(/* zeroTerminated */ false, /* clear */ false, sizeof(long long int));

	return allocator;
}

long long int shovelerEntityIdAllocatorAllocate(ShovelerEntityIdAllocator* allocator)
{
	long long int* entityId = malloc(sizeof(long long int));
	if (allocator->freeList->len > 0) {
		*entityId = g_array_index(allocator->freeList, long long int, 0);
		g_array_remove_index_fast(allocator->freeList, 0);
	} else {
		*entityId = allocator->nextFreshEntityId++;
	}

	g_hash_table_add(allocator->allocations, entityId);
	return *entityId;
}

bool shovelerEntityIdAllocatorDeallocate(ShovelerEntityIdAllocator* allocator, long long int entityId)
{
	if (!g_hash_table_remove(allocator->allocations, &entityId)) {
		return false;
	}

	g_array_append_val(allocator->freeList, entityId);
	return true;
}

void shovelerEntityIdAllocatorFree(ShovelerEntityIdAllocator* allocator)
{
	g_array_free(allocator->freeList, /* freeSegment */ true);
	g_hash_table_destroy(allocator->allocations);
	free(allocator);
}
