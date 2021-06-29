#include <gdt.h>
#include <bootcon.h>
extern G2BootConsole console;

void initGDT(SegmentDescriptor *gdt, int descriptors) {
	GDTRegister gdtr;
	// intStatus((int)gdt);
	// termPrint("Global Descriptor Table address\n");

	gdt[0] = createDescriptor(0, 0, 0);
	gdt[1] = createDescriptor(0, 0xFFFFF, (GDT_CODE_PL0));
	gdt[2] = createDescriptor(0, 0xFFFFF, (GDT_DATA_PL0));
	gdt[3] = createDescriptor(0, 0xFFFFF, (GDT_CODE_PL3));
	gdt[4] = createDescriptor(0, 0xFFFFF, (GDT_DATA_PL3));

	gdtr.base = gdt;
	gdtr.size = 8 * descriptors - 1;

	__asm__ __volatile__ ("lgdt (%0)": : "r" (&gdtr));

    __asm__ __volatile__ (
        "sub $16, %rsp \n"
        "movq $8, 8(%rsp) \n"
        "movabsq $flushGDT, %rax \n"
        "mov %rax, (%rsp) \n"
        "lretq \n"
        "flushGDT: \n"
        "mov $0x10, %ax \n"
        "mov %ax, %ds \n"
        "mov %ax, %es \n"
        "mov %ax, %ss \n"
    );
	// flushGDT();
}


