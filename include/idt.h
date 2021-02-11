#if !defined(_IDT_H_)
#define _IDT_H_
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// This is the Intel-defined structure for an IDT entry
// Awful, but not my fault
typedef struct id {
	uint16_t offsetLow;
	uint16_t selector;
	uint8_t ist;
	uint8_t type;
	uint16_t offsetMid;
	uint32_t offsetHigh;
	uint32_t mustBeZero;
} __attribute__((packed)) InterruptDescriptor;

// Unchanged from GenOS 1 because pointers are bigger now
typedef struct idtr {
	uint16_t limit;
	InterruptDescriptor *base;
} __attribute__((packed)) IDTRegister;

class G2IDT {
public:
	G2IDT(InterruptDescriptor *idt, size_t size);

	// This is carried over from GenOS 1 -
	// back then, you had to wrap your ISR's with assembly
	// so GCC wouldn't choke
	// Now, if you use __attribute__((interrupt)),
	// you should be able to stuff your functions directly into "wrapper"
	void registerHandler(int vector, void *wrapper,
		bool present, uint8_t dpl, uint8_t type);

private:
	InterruptDescriptor *idt;
	size_t size;
};
#define TASK_GATE 5
#define INTERRUPT_GATE 14
#define TRAP_GATE 15

#endif
