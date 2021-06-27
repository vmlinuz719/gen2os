#include <Platform/PCx64_Uniprocessor/8259.h>
#include <Platform/PCx64_Uniprocessor/Common.h>


Pc8259::Pc8259(uint8_t leaderOffset, uint8_t followerOffset) {
	// remap the PIC in the constructor
	// don't trust BOOTBOOT to do it for us!
	uint8_t leaderMask, followerMask;
	
	leaderMask = inb(PIC1_DATA);
	followerMask = inb(PIC2_DATA);
	
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	ioWait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	ioWait();
	
	outb(PIC1_DATA, leaderOffset);
	ioWait();
	outb(PIC2_DATA, followerOffset);
	ioWait();
	
	outb(PIC1_DATA, 4);
	ioWait();
	outb(PIC2_DATA, 2);
	ioWait();
	
	outb(PIC1_DATA, ICW4_8086);
	ioWait();
	outb(PIC2_DATA, ICW4_8086);
	ioWait();
	
	outb(PIC1_DATA, leaderMask);
	outb(PIC2_DATA, followerMask);
}

void Pc8259::EOI(uint8_t irq) {
	if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

void Pc8259::setMask(uint8_t line) {
	uint16_t port;
    uint8_t value;
 
    if (line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        line -= 8;
    }
    value = inb(port) | (1 << line);
    outb(port, value);
}

void Pc8259::clearMask(uint8_t line) {
	uint16_t port;
    uint8_t value;
 
    if(line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        line -= 8;
    }
    value = inb(port) & ~(1 << line);
    outb(port, value);
}

void Pc8259::maskAll(void) {
	for (int x = 0; x < MAX_IRQ; x++) {
		setMask(x);
	}
}

// osdev
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t Pc8259::getISR(void) {
	return __pic_get_irq_reg(PIC_READ_ISR);
}

uint16_t Pc8259::getIRR(void) {
	return __pic_get_irq_reg(PIC_READ_IRR);
}
