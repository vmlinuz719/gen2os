#include <heap.h>

// TODO: We allegedly need to make sure we align our allocations
// this has never been a problem for me but on some systems, it's
// an error, undefined behavior, blah blah blah

void initHeapDescriptor(HeapDescriptor *hd,
	HeapDescriptor *prev, HeapDescriptor *next, bool usable) {
	hd->magic = HEAP_MAGIC;
	hd->prev = prev;
	hd->next = next;
	hd->used = false;
	hd->usable = usable;
}

size_t descriptorSize(HeapDescriptor *hd) {
	// This returns the size of a heap descriptor
	// INCLUDING the descriptor itself
	if (hd == NULL || hd->next == NULL) return 0;

	if (hd->magic == HEAP_MAGIC && hd->next->magic == HEAP_MAGIC
		&& hd->usable) {
		return (uint8_t *)(hd->next)
			- (uint8_t *)hd;
	}

	return 0;
}

size_t descriptorDataSize(HeapDescriptor *hd) {
	// This just gets the size of the data
	size_t result = descriptorSize(hd);

	if (result == 0) return result;
	return result - sizeof(HeapDescriptor);
}

Heap::Heap(uint8_t *initialSpace, size_t size) {
	// Write heap descriptors into the initial buffer
	HeapDescriptor *start = (HeapDescriptor *)initialSpace;
	// Compute the end of the heap - can't run off the edge of the buffer!
	HeapDescriptor *end =
		(HeapDescriptor *)(initialSpace + size - sizeof(HeapDescriptor));

	initHeapDescriptor(start, NULL, end, true);
	initHeapDescriptor(end, start, NULL, false);

	heapStart = start;
	heapEnd = end;
	lastFree = NULL;
}

bool Heap::extend(uint8_t *memory, size_t size) {
	// Do nothing if you haven't provided enough space
	if (size < 2 * sizeof(HeapDescriptor)) {
		return false;
	}
	
	// Write heap descriptors into the initial buffer
	HeapDescriptor *start = (HeapDescriptor *)memory;
	// Compute the end of the heap - can't run off the edge of the buffer!
	HeapDescriptor *end =
		(HeapDescriptor *)(memory + size - sizeof(HeapDescriptor));

	initHeapDescriptor(start, heapEnd, end, true);
	initHeapDescriptor(end, start, NULL, false);
	
	heapEnd->next = start;
	heapEnd = end;
	
	return true;
}

void *Heap::malloc(size_t size) {
	// Where ever shall we find our memory allocation?
	HeapDescriptor *hd = NULL;
	// If it will fit, reuse the last freed block
	// Best case is thus O(1)
	if (lastFree != NULL && lastFree->magic == HEAP_MAGIC
		&& lastFree->usable && !(lastFree->used)
		&& size <= descriptorDataSize(lastFree)) {
		hd = lastFree;
	}
	else {
		// go fish
		// iterate through all heap descriptors
		// until we find one that works
		hd = heapStart;
		while (hd != NULL &&
			(!(hd->usable)
			|| hd->used
			|| descriptorDataSize(hd) < size)) {
			if (hd->magic != HEAP_MAGIC) return NULL;
			hd = hd->next;
		}
	}

	if (hd == NULL || hd->magic != HEAP_MAGIC) {
		// termPrint("kMalloc failure!\n");
		return NULL;
	}

	hd->used = true;

	if (descriptorDataSize(hd) > size + sizeof(HeapDescriptor)) {
		HeapDescriptor *desc =
			(HeapDescriptor *)
			((uint8_t *)hd + size + sizeof(HeapDescriptor));
		hd->next->prev = desc;
		initHeapDescriptor(desc, hd, hd->next, true);
		hd->next = desc;
		// termPrint("kMalloc split\n");
	}

	return (void *)((uint8_t *)hd + sizeof(HeapDescriptor));
}

void Heap::free(void *ptr) {
	HeapDescriptor *hd =
		(HeapDescriptor *)((uint8_t *)ptr - sizeof(HeapDescriptor));

	// first, mark the block as unused
	hd->used = false;

	HeapDescriptor *prevFree = hd;
	HeapDescriptor *nextFree = hd;

	// stick together surrounding unused blocks
	while (prevFree->prev != NULL
		&& prevFree->prev->usable
		&& !(prevFree->prev->used)) {
		// termPrint("zip down\n");
		prevFree = prevFree->prev;
	}

	while (nextFree->next != NULL
		&& nextFree->next->usable
		&& !(nextFree->next->used)) {
		// termPrint("zip up\n");
		nextFree = nextFree->next;
	}

	// invalidate the descriptor if it got coalesced
	if (prevFree != nextFree) {
		// hd->magic = 0;

		prevFree->next = nextFree->next;
		nextFree->next->prev = prevFree;
	}
	
	// save the final free block for quick reuse
	lastFree = prevFree;
}
