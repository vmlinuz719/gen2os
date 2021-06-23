#include <stdint.h>
#include "bitmap.h"

// testBit: test any bit in a uint8_t array

int testBit(uint8_t *bitfield, size_t bit) {
	size_t byteIndex = bit / 8;
	uint8_t bitIndex = 7 - (bit % 8);
	
	return bitfield[byteIndex] & (1 << bitIndex);
}

// setBit: set any bit in a uint8_t array

void setBit(uint8_t *bitfield, size_t bit) {
	size_t byteIndex = bit / 8;
	uint8_t bitIndex = 7 - (bit % 8);
	
	bitfield[byteIndex] |= (1 << bitIndex);
}

// clearBit: guess

void clearBit(uint8_t *bitfield, size_t bit) {
	size_t byteIndex = bit / 8;
	uint8_t bitIndex = 7 - (bit % 8);
	
	bitfield[byteIndex] &= ~(1 << bitIndex);
}

// setBitBlock: writes bits to an arbitrary bit offset

void setBitBlock(uint8_t *bitfield, size_t startBit, size_t nBits, uint64_t value) {
	for (int i = nBits - 1; i >= 0; i--) {
		if (value & 1)
			setBit(bitfield, startBit + i);
		else
			clearBit(bitfield, startBit + i);
		value >>= 1;
	} 
}

// commonInit: private method to initialize a Bitmap
// Use the offset of the region to be mapped and its size in 4096 byte blocks

void Bitmap::commonInit(uint8_t *offset, size_t numBlocks) {
	// Calculate some useful numbers for the size of the bitmap
	size_t bitmapSizeBytes = numBlocks / 8 + (numBlocks % 8 ? 1 : 0);
	bitmapSizeBlocks = bitmapSizeBytes / 4096 + (bitmapSizeBytes % 4096 ? 1 : 0);
	
	// Initialize the bitmap to 0
	for (size_t i = 0; i < bitmapSizeBytes; i++) {
		offset[i] = 0;
	}
	// Mark the bitmap as used so we won't attempt to spray over it
	for (size_t i = 0; i < bitmapSizeBlocks; i++) {
		setBit(offset, i);
	}
}

// Two constructors that both call commonInit

Bitmap::Bitmap(Allocation a) {
	region = a;
	commonInit(region.offset, region.numBlocks);
}

Bitmap::Bitmap(uint8_t *offset, size_t numBlocks) {
	region.offset = offset;
	region.numBlocks = numBlocks;
	commonInit(offset, numBlocks);
}

// Mark some number of blocks for use
// Returns: The address of the blocks that were allocated,
// plus the number of blocks
// We may not return the full number of blocks requested if we cannot find a
// contiguous region; you may need to call it again
// A returned allocation of size 0 indicates no memory left

Allocation Bitmap::allocate(size_t numBlocksRequested) {
	uint64_t maxBlockStart = 0;
	size_t maxBlockSize = 0;
	
	Allocation result;
	result.offset = 0;
	result.numBlocks = 0;
	
	uint64_t currentBlockStart = 0;
	size_t currentBlockSize = 0;
	
	// Start seeking at the beginning of the bitmap
	while (currentBlockStart < region.numBlocks) {
		if (!testBit(region.offset, currentBlockStart)) {
			// we found a free block, let's see if there are more
			while (currentBlockStart + currentBlockSize < region.numBlocks
				&& !testBit(region.offset,
					currentBlockStart + currentBlockSize)) {
				currentBlockSize++;
			}
			
			// keep track of the biggest region we've found so far
			if (currentBlockSize > maxBlockSize) {
				maxBlockStart = currentBlockStart;
				maxBlockSize = currentBlockSize;
			}
			
			// if it's big enough, we just allocate what we need and return
			if (currentBlockSize >= numBlocksRequested) {
				for (size_t i = 0; i < numBlocksRequested; i++) {
					setBit(region.offset, currentBlockStart + i);
				}
				
				result.offset = region.offset + currentBlockStart * 4096;
				result.numBlocks = numBlocksRequested;
				return result;
			}
			
			currentBlockStart += currentBlockSize;
			currentBlockSize = 0;
		} else {
			currentBlockStart++;
		}
	}
	
	// we didn't find a region big enough,
	// but fear not, we will return the biggest one we found
	for (size_t i = 0; i < maxBlockSize; i++) {
		setBit(region.offset, maxBlockStart + i);
	}
	
	result.offset = region.offset + maxBlockStart * 4096;
	result.numBlocks = maxBlockSize;
	return result;
}

// Free an allocated block of memory
// Returns: 0 on success, 1 on free out of bounds, 2 on double free

int Bitmap::free(uint8_t *offset, size_t numBlocks) {
	size_t start = ((size_t) (offset - region.offset)) / 4096;
	
	if (start < bitmapSizeBlocks
		|| start + numBlocks - 1 >= region.numBlocks) {
		// attempted free out of bounds;
		// it is an error to attempt to free the bitmap page itself
		return 1;
	}
	
	for (size_t i = start; i <= start + numBlocks - 1; i++) {
		if (!testBit(region.offset, i)) {
			// attempted double free
			return 2;
		}
	}
	
	// bounds check and double-free check successful, let's free it
	for (size_t i = start; i <= start + numBlocks - 1; i++) {
		clearBit(region.offset, i);
	}
	
	return 0;
}

// same thing really

int Bitmap::free(Allocation a) {
	return free(a.offset, a.numBlocks);
}
