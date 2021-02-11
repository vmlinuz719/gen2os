#include <bootcon.h>
#include <8259.h>
#include <kbdtest.h>
#include <port.h>

extern G2BootConsole *console;
extern G2PIC *pic8259;

void int2Hex(uint64_t x, char *buf, size_t size) {
	// expect buf to be n characters long - last one gets \0
	buf[size - 1] = '\0';

	uint64_t num = x;
	int i = size - 1;

	// (void)x;
	do {
		buf[--i] = "0123456789ABCDEF"[num % 16];
		num >>= 4;
	} while (num > 0 && i);

	while (--i >= 0) buf[i] = '0';
}

__attribute__((interrupt)) void kbdTest(ExceptionStackFrame *frame) {
	console->puts("Keyboard event!\n");
	G2Inline::inb(0x60);
	pic8259->EOI(1);
	asm("sti");
	return;
}

__attribute__((interrupt)) void KIWF(ExceptionStackFrame *frame, unsigned long int errorCode) {
	console->puts("SYS$BUGCHECK -- KERNEL MODE FATAL EXCEPTION; HALT NOW\n");
	
	char buf[17];
	int2Hex(frame->RIP, buf, sizeof(buf));
	console->puts(buf);
	
	asm("cli");
	asm("hlt");
	return;
}

__attribute__((interrupt)) void illegal(ExceptionStackFrame *frame) {
	console->puts("SYS$ILLINSTR -- ILLEGAL INSTRUCTION @ 0x");
	
	char buf[17];
	int2Hex(frame->RIP, buf, sizeof(buf));
	console->puts(buf);
	
	console->puts("\n");
	return;
}

__attribute__((interrupt)) void generalProtection(ExceptionStackFrame *frame, unsigned long int errorCode) {
	console->puts("SYS$GPRFAULT -- GENERAL PROTECTION FAULT 0x");
	
	char buf[17];
	int2Hex(errorCode, buf, sizeof(buf));
	console->puts(buf);
	
	console->puts("\n");
	return;
}
