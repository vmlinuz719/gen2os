/*
 * mykernel/cpp/kernel.cpp
 *
 * Copyright (C) 2017 - 2021 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This file is part of the BOOTBOOT Protocol package.
 * @brief A sample BOOTBOOT compatible kernel
 *
 */

#include <stdint.h>
#include <bootboot.h>
#include <heap.h>
#include <bootcon.h>
#include <8259.h>
#include <port.h>
#include <idt.h>
#include <kbdtest.h>
#include <gdt.h>
#include <ksync.h>
#include <cpuid.h>

char *int2Hex(uint64_t x, char *buf, size_t size, int radix) {
	// expect buf to be n characters long - last one gets \0
	buf[size - 1] = '\0';

	uint64_t num = x;
	int i = size - 1;

	// (void)x;
	do {
		buf[--i] = "0123456789ABCDEF"[num % radix];
		num /= radix;
	} while (num > 0 && i);

    int j = i;

	while (--i >= 0) buf[i] = '0';

	return buf + j;
}

uint8_t initHeap[65535];

/* imported virtual addresses, see linker script */
extern BOOTBOOT bootboot;               // see bootboot.h
extern unsigned char environment[4096]; // configuration, UTF-8 text key=value pairs
extern uint8_t fb;                      // linear framebuffer mapped

typedef void (*xtor)();
extern "C" xtor start_ctors;
extern "C" xtor end_ctors;
extern "C" xtor start_dtors;
extern "C" xtor end_dtors;

extern "C" void globalCtors() {
    for (xtor *i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

// don't bother with global dtors

SegmentDescriptor gdt[8];

extern G2BootConsole console;

class G2Kernel {

public:
    
    G2Kernel() {
        /*** NOTE: this code runs on all cores in parallel ***/
        
        int x, y, s=bootboot.fb_scanline;

        if(s) {

            // red, green, blue boxes in order
            for(y=0;y<20;y++) { for(x=0;x<20;x++) { *((uint32_t*)(&fb + s*(y+20) + (x+20)*4))=0x00FF0000; } }
            for(y=0;y<20;y++) { for(x=0;x<20;x++) { *((uint32_t*)(&fb + s*(y+20) + (x+50)*4))=0x0000FF00; } }
            for(y=0;y<20;y++) { for(x=0;x<20;x++) { *((uint32_t*)(&fb + s*(y+20) + (x+80)*4))=0x000000FF; } }			
        }
    }
    
    void init(void) {
        console.puts("GEN/2 System Product\n\(C) 2021 vmlinuz719. All rights reserved. \n\n");

        char buf[17];

        int2Hex(bootboot.initrd_ptr, buf, sizeof(buf), 16);
        console.puts("IMG: addr 0x");
        console.puts(buf);

        int2Hex(bootboot.initrd_size, buf, sizeof(buf), 10);
        console.puts(" size ");
        console.puts(buf);

        int2Hex(bootboot.initrd_size + bootboot.initrd_ptr, buf, sizeof(buf), 16);
        console.puts(" limit 0x");
        console.puts(buf);

        console.puts("\n");

        MMapEnt *mmap_ent = &bootboot.mmap;
        for (unsigned int i = 0; i < (bootboot.size - 128) / 16; i++) {
            uint64_t ptr = (mmap_ent + i)->ptr;
            uint64_t size = (mmap_ent + i)->size;
            console.puts("MEM: ");

            int2Hex(ptr, buf, sizeof(buf), 16);
            console.puts("addr 0x");
            console.puts(buf);

            uint64_t actualSize = size & 0xFFFFFFFFFFFFFFF0;

            int2Hex(actualSize, buf, sizeof(buf), 10);
            console.puts(" size ");
            console.puts(buf);

            int2Hex(actualSize + ptr, buf, sizeof(buf), 16);
            console.puts(" limit 0x");
            console.puts(buf);

            console.puts(" type ");
            switch(size & 0xF) {
                case MMAP_USED:
                    console.puts("USED");
                    break;
                case MMAP_FREE:
                    console.puts("FREE");
                    break;
                case MMAP_ACPI:
                    console.puts("ACPI");
                    break;
                case MMAP_MMIO:
                    console.puts("MMIO");
                    break;
            }

            console.puts("\n");
        }
     }
    
    void hang(void) {
    	console.puts(" -- ALL AVAILABLE FUNCTIONS COMPLETED\n");
    	while(1) {asm("hlt");};
    }

    /* destructor */
    ~G2Kernel() {
    }
};

/******************************************
 * Entry point, called by BOOTBOOT Loader *
 ******************************************/

static G2Kernel kernel = G2Kernel();
G2BootConsole console = G2BootConsole(&fb, (psf2_t *)&_binary_font_psf_start,
    bootboot.fb_width, bootboot.fb_height, bootboot.fb_scanline);

int main() {
    // We want to halt all but the bootstrap processor for now
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    unsigned char localApicId = (ebx & 0xFF000000) >> 24;
    if (localApicId != bootboot.bspid) {
        while(1) {asm("hlt");};
    }

    globalCtors();

    kernel.init();
    kernel.hang();

    return 0;
}
