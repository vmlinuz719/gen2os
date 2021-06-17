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

G2BootConsole *console;
G2PIC *pic8259;

class G2Kernel {

public:
	Heap heap = Heap(initHeap, 65535);
    
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
    	console = (G2BootConsole *)heap.malloc(sizeof(G2BootConsole));
    	*console = G2BootConsole(&fb, (psf2_t *)&_binary_font_psf_start,
    		bootboot.fb_width, bootboot.fb_height, bootboot.fb_scanline);
    	
        console->puts("GEN/2 System Product\n\(C) 2021 vmlinuz719. All rights reserved. \n\n");
     }
    
    void hang(void) {
    	console->puts("SYS$COMPLETE -- ALL AVAILABLE FUNCTIONS COMPLETED\n");
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

int main() {
    globalCtors();
    kernel.init();
    kernel.hang();
    return 0;
}
