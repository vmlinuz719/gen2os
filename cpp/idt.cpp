#include <idt.h>
#include <port.h>

G2IDT::G2IDT(InterruptDescriptor *idt, size_t size) {
	G2Inline::clearInts();
	
	IDTRegister idtr;
	this->size = idtr.limit = sizeof(InterruptDescriptor) * size;
	this->idt = idtr.base = idt;
	
	__asm__ __volatile__ ("lidt (%0)": : "r" (&idtr));
}

void G2IDT::registerHandler(int vector, void *wrapper, uint8_t selector,
		bool present, uint8_t dpl, uint8_t type) {
	G2Inline::clearInts();
	
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
	
	G2Inline::setInts();
}
