#if !defined(_HEAP_H_)
#define _HEAP_H_

#include <stdint.h>
#include <stddef.h>

#define HEAP_MAGIC 0x1C07FEFE // covfefe

typedef struct hd {
	struct hd *prev;
	struct hd *next;
	uint32_t magic;
	bool used;
	bool usable;
} HeapDescriptor;

class Heap {
public:
	Heap(uint8_t *initialSpace, size_t size);
	void *malloc(size_t size);
	void free(void *ptr);
	
	bool extend(uint8_t *memory, size_t size);
	
	HeapDescriptor *getHeapStart() { return heapStart; }
	HeapDescriptor *getHeapEnd() { return heapEnd; }
	HeapDescriptor *getLastFree() { return lastFree; }

private:
	HeapDescriptor *heapStart;
	HeapDescriptor *heapEnd;
	HeapDescriptor *lastFree;
};

#endif
