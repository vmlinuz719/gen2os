#include <8259.h>
#include <port.h>


G2PIC::G2PIC(uint8_t leaderOffset, uint8_t followerOffset) {
	// remap the PIC in the constructor
	// don't trust BOOTBOOT to do it for us!
	uint8_t leaderMask, followerMask;
	
	leaderMask = G2Inline::inb(PIC1_DATA);
	followerMask = G2Inline::inb(PIC2_DATA);
	
	G2Inline::outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	G2Inline::ioWait();
	G2Inline::outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	G2Inline::ioWait();
	
	G2Inline::outb(PIC1_DATA, leaderOffset);
	G2Inline::ioWait();
	G2Inline::outb(PIC2_DATA, followerOffset);
	G2Inline::ioWait();
	
	G2Inline::outb(PIC1_DATA, 4);
	G2Inline::ioWait();
	G2Inline::outb(PIC2_DATA, 2);
	G2Inline::ioWait();
	
	G2Inline::outb(PIC1_DATA, ICW4_8086);
	G2Inline::ioWait();
	G2Inline::outb(PIC2_DATA, ICW4_8086);
	G2Inline::ioWait();
	
	G2Inline::outb(PIC1_DATA, leaderMask);
	G2Inline::outb(PIC2_DATA, followerMask);
}

void G2PIC::EOI(uint8_t irq) {
	if (irq >= 8) G2Inline::outb(PIC2_COMMAND, PIC_EOI);
	G2Inline::outb(PIC1_COMMAND, PIC_EOI);
}

void i8259SetMask(uint8_t line) {
	uint16_t port;
    uint8_t value;
 
    if (line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        line -= 8;
    }
    value = G2Inline::inb(port) | (1 << line);
    G2Inline::outb(port, value);
}

void G2PIC::clearMask(uint8_t line) {
	uint16_t port;
    uint8_t value;
 
    if(line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        line -= 8;
    }
    value = G2Inline::inb(port) & ~(1 << line);
    G2Inline::outb(port, value);
}

void G2PIC::maskAll(void) {
	for (int x = 0; x < MAX_IRQ; x++) {
		i8259SetMask(x);
	}
}

// osdev
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    G2Inline::outb(PIC1_COMMAND, ocw3);
    G2Inline::outb(PIC2_COMMAND, ocw3);
    return (G2Inline::inb(PIC2_COMMAND) << 8) | G2Inline::inb(PIC1_COMMAND);
}

uint16_t G2PIC::getISR(void) {
	return __pic_get_irq_reg(PIC_READ_ISR);
}

uint16_t G2PIC::getIRR(void) {
	return __pic_get_irq_reg(PIC_READ_IRR);
}
