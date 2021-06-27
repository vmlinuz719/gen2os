#include <Platform/PCx64_Uniprocessor/idt.h>
#include <Platform/PCx64_Uniprocessor/Common.h>

PcIDT::PcIDT() {
	asm("cli");
	
	IDTRegister idtr;
	idtr.limit = sizeof(InterruptDescriptor) * NUM_DESCR - 1;
	idtr.base = idt;
	
	asm volatile ("lidt (%0)": : "r" (&idtr));
}

void PcIDT::registerHandler(int vector, void *wrapper, uint8_t selector,
		bool present, uint8_t dpl, uint8_t type) {
	asm("cli");
	
	uint64_t dfcall = (uint64_t)wrapper;
	
	uint8_t t = 0;
	
	t |= ((present ? 1 : 0) & 1) << 7;
	t |= (dpl & 3) << 5;
	// storage must be zero
	t |= type & 0x0F;
	
	idt[vector].offsetLow = dfcall & 0xffff;
	idt[vector].selector = selector;
	idt[vector].mustBeZero = idt[vector].ist = 0;
	idt[vector].type = t; /* INTERRUPT_GATE */
	idt[vector].offsetMid = (dfcall & 0xffff0000) >> 16;
	idt[vector].offsetHigh = (dfcall & 0xffffffff00000000) >> 32;
	
	asm("sti");
}
