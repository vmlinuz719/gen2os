#include <gdt.h>

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
	gdtr.size = 32 * descriptors - 1;

	__asm__ __volatile__ ("lgdt (%0)": : "r" (&gdtr));
	// flushGDT();
}


