#if !defined(_BITMAP_H_)
#define _BITMAP_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t *offset;
	size_t numBlocks; // number of 4k blocks or pages
} Allocation;

int testBit(uint8_t *bitfield, size_t bit);
void setBit(uint8_t *bitfield, size_t bit);
void clearBit(uint8_t *bitfield, size_t bit);
void setBitBlock(uint8_t *bitfield, size_t startBit, size_t nBits, uint64_t value);

class Bitmap {
public:	
	Bitmap(Allocation a);
	Bitmap(uint8_t *offset, size_t numBlocks);
	
	Allocation allocate(size_t numBlocksRequested);
	int free(uint8_t *offset, size_t numBlocks);
	int free(Allocation a);
	
private:
	Allocation region;
	size_t bitmapSizeBlocks;
	
	void commonInit(uint8_t *offset, size_t numBlocks);
};

#endif
