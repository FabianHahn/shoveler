#include "shoveler/collider.h"

const ShovelerCollider2 *shovelerCollider2IntersectFiltered(const ShovelerCollider2 *collider, const ShovelerBoundingBox2 *object, ShovelerCollider2FilterCandidateFunction *filterCandidate, void *filterCandidateUserData)
{
	if(shovelerBoundingBox2Intersect(&collider->boundingBox, object)) {
		const ShovelerCollider2 *candidate = collider;
		if(collider->intersect != NULL) {
			candidate = collider->intersect(collider, object, filterCandidate, filterCandidateUserData);
		}

		if(filterCandidate != NULL && !filterCandidate(candidate, filterCandidateUserData)) {
			return NULL;
		}

		return candidate;
	}
	return NULL;
}

const ShovelerCollider3 *shovelerCollider3IntersectFiltered(const ShovelerCollider3 *collider, const ShovelerBoundingBox3 *object, ShovelerCollider3FilterCandidateFunction *filterCandidate, void *filterCandidateUserData)
{
	if(shovelerBoundingBox3Intersect(&collider->boundingBox, object)) {
		const ShovelerCollider3 *candidate = collider;
		if(collider->intersect != NULL) {
			candidate = collider->intersect(collider, object, filterCandidate, filterCandidateUserData);
		}

		if(filterCandidate != NULL && !filterCandidate(candidate, filterCandidateUserData)) {
			return NULL;
		}

		return candidate;
	}
	return NULL;
}
