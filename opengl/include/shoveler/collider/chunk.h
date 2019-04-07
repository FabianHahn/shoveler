#ifndef SHOVELER_COLLIDER_CHUNK_H
#define SHOVELER_COLLIDER_CHUNK_H

struct ShovelerChunkStruct; // forward declaration: chunk.h
struct ShovelerCollider2Struct; // forward declaration: collider.h

struct ShovelerCollider2Struct *shovelerColliderChunkCreate(struct ShovelerChunkStruct *chunk);

#endif
