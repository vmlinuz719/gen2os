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
#include <bitmap.h>

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
InterruptDescriptor idt[64];

extern G2BootConsole console;
extern G2PIC pic8259;

Bitmap bmp((uint8_t *) 0, 0);

class G2Kernel {

public:
    
    G2IDT table = G2IDT(idt, 64);

    G2Kernel() {
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
        char *sz;

        initGDT(gdt, 8);
        pic8259.maskAll();

        int2Hex(bootboot.initrd_ptr, buf, sizeof(buf), 16);
        console.puts("System image @ 0x");
        console.puts(buf);

        sz = int2Hex(bootboot.initrd_size, buf, sizeof(buf), 10);
        console.puts(" size ");
        console.puts(sz);

        console.puts("\n");

        MMapEnt *mmap_ent = &bootboot.mmap;
        uint64_t maxptr = 0;
        size_t maxsize = 0;

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
                    if (actualSize > maxsize) {
                        maxsize = actualSize;
                        maxptr = ptr;
                    }
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

        sz = int2Hex(maxsize / 4096, buf, sizeof(buf), 10);
        console.puts("BMP: Using ");
        console.puts(sz);
        int2Hex(maxptr, buf, sizeof(buf), 16);
        console.puts(" physical blocks at 0x");
        console.puts(buf);
        console.puts("\n");

        bmp = Bitmap((uint8_t *) maxptr, (size_t) (maxsize / 4096));

        uint64_t initCR3 = 0;
        uint64_t *initPML4;
        asm("mov %%cr3,%0" : "=r"(initCR3));
        initPML4 = (uint64_t *) (initCR3 & ~(0xFFFL));
        int2Hex((uint64_t) initPML4, buf, sizeof(buf), 16);
        console.puts("Initial PML4 @ 0x");
        console.puts(buf);
        console.puts("\n");

        // Let's try rebuilding the PML4

        uint64_t *newPML4;
        Allocation a = bmp.allocate(1);
        newPML4 = (uint64_t *) a.offset;
        for (int i = 0; i < 256; i++) {
            newPML4[i] = 0;
        }
        for (int i = 256; i < 512; i++) {
            newPML4[i] = initPML4[i];
        }

        int2Hex((uint64_t) newPML4, buf, sizeof(buf), 16);
        console.puts("New PML4 @ 0x");
        console.puts(buf);
        console.puts("\n");

        asm("mov %0, %%cr3" :: "r"(newPML4));

        table.registerHandler(0x21, (void *)kbdTest, 0x08, true, 0, INTERRUPT_GATE);
        pic8259.clearMask(1);

        G2Inline::setInts();
    }
    
    void hang(void) {
    	console.puts("\n -- ALL AVAILABLE FUNCTIONS COMPLETED\n");
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
G2PIC pic8259 = G2PIC(32, 48);

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
