#if !defined (_Pc8259_)
#define _Pc8259_
#include <stddef.h>
#include <stdint.h>

#define PIC1		0x20		/* IO base address for leader PIC */
#define PIC2		0xA0		/* IO base address for follower PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_FOLLOWER	0x08		/* Buffered mode/follower */
#define ICW4_BUF_LEADER	0x0C		/* Buffered mode/leader */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */

#define PIC_EOI		0x20
#define MAX_IRQ 16

#define LEADER_PIC_REMAP 0x30
#define FOLLOWER_PIC_REMAP 0x38

class Pc8259 {
public:
    /* Consult the Intel 8259 PIC documentation to figure out what these do.
     * It's pretty standard stuff
     */
	Pc8259(uint8_t leaderOffset, uint8_t followerOffset);
	void EOI(uint8_t irq);
	void setMask(uint8_t line);
	void clearMask(uint8_t line);
	uint16_t getISR(void);
	uint16_t getIRR(void);
	void maskAll(void);
};

#endif

