#include <stdint.h>
#include <Platform/PCx64_Uniprocessor/idt.h>
#include <Platform/PCx64_Uniprocessor/8259.h>
#include <Platform/PCx64_Uniprocessor/Interrupt.h>
#include <Platform/PlInterrupt.h>

/* PCx64_Uniprocessor/Interrupt.h
 * This class implements PlInterrupt for the x86_64 uniprocessor Platform
 * Layer. It initializes the IDT and provides a (restricted) interface for
 * working with the x86's interrupt facilities.
 */

PcInterrupt::PcInterrupt(uint8_t io0, uint8_t io1)
    : PlInterrupt()
    , pic(io0, io1)
    , idt() {
    irqOffset0 = io0;
    irqOffset1 = io1;

    for (int i = 0; i < MAX_IRQ; i++) {
        irqHandlers[i] = (void *) 0;
    }
    for (int i = 0; i < MAX_EXC; i++) {
        excHandlers[i] = (void *) 0;
    }
}

void PcInterrupt::PlIntDisable() { __asm__ __volatile__ ("cli"); }
void PcInterrupt::PlIntEnable() { __asm__ __volatile__ ("sti"); }

int PcInterrupt::PlIrqRegisterHandler
    (int irq, void *handlerFn, bool allowNested, bool protect) {
    // error checks
    if (irq < 0 || irq >= MAX_IRQ)
        return PL_INT_IRQ_OUT_OF_RANGE;
    if (irqHandlers[irq] != (void *) 0)
        return PL_INT_IRQ_HANDLER_PRESENT;

    irqHandlers[irq] = handlerFn;

    int vector = 0;
    if (irq >= MAX_IRQ / 2)
        vector = irq + irqOffset1;
    else vector = irq + irqOffset0;

    uint8_t dpl = 0;
    if (!protect)
        dpl = 3;

    uint8_t type = INTERRUPT_GATE;
    if (allowNested)
        type = TRAP_GATE;

    idt.registerHandler(vector, handlerFn, 8, true, dpl, type);
    return PL_INT_SUCCESS;
}

void *PcInterrupt::PlIrqQueryHandler(int irq) {
    if (irq < 0 || irq >= MAX_IRQ)
        return (void *) 0;

    return irqHandlers[irq];
}

void PcInterrupt::PlIrqMask(int irq) {
    if (irq < 0 || irq >= MAX_IRQ)
        return;

    pic.setMask((uint8_t) irq);
}

void PcInterrupt::PlIrqUnmask(int irq) {
    if (irq < 0 || irq >= MAX_IRQ)
        return;

    pic.clearMask((uint8_t) irq);
}

void PcInterrupt::PlIrqMaskAll() {
    pic.maskAll();
}

int PcInterrupt::PlIrqFlushHandler(int irq) {
    // error checks
    if (irq < 0 || irq >= MAX_IRQ)
        return PL_INT_IRQ_OUT_OF_RANGE;
    if (irqHandlers[irq] == (void *) 0)
        return PL_INT_IRQ_HANDLER_NOT_PRESENT;

    int vector = 0;
    if (irq >= MAX_IRQ / 2)
        vector = irq + irqOffset1;
    else vector = irq + irqOffset0;

    idt.registerHandler(vector, (void *) 0, 0, false, 0, 0);
    irqHandlers[irq] = (void *) 0;
    return PL_INT_SUCCESS;
}

void PcInterrupt::PlIrqEOI(int irq) {
    if (irq < 0 || irq >= MAX_IRQ)
        return;

    pic.EOI(irq);
}

int PcInterrupt::PlExcRegisterHandler
    (int exc, void *handlerFn, bool allowNested, bool protect) {
    // error checks
    if (exc < 0 || exc >= MAX_EXC)
        return PL_INT_EXC_OUT_OF_RANGE;
    if (excHandlers[exc] != (void *) 0)
        return PL_INT_EXC_HANDLER_PRESENT;

    excHandlers[exc] = handlerFn;

    int vector = exc;

    uint8_t dpl = 0;
    if (!protect)
        dpl = 3;

    uint8_t type = INTERRUPT_GATE;
    if (allowNested)
        type = TRAP_GATE;

    idt.registerHandler(vector, handlerFn, 8, true, dpl, type);
    return PL_INT_SUCCESS;
}

void *PcInterrupt::PlExcQueryHandler(int exc) {
    if (exc < 0 || exc >= MAX_EXC)
        return (void *) 0;

    return excHandlers[exc];
}

int PcInterrupt::PlExcFlushHandler(int exc) {
    // error checks
    if (exc < 0 || exc >= MAX_EXC)
        return PL_INT_EXC_OUT_OF_RANGE;
    if (excHandlers[exc] == (void *) 0)
        return PL_INT_EXC_HANDLER_NOT_PRESENT;

    int vector = exc;

    idt.registerHandler(vector, (void *) 0, 0, false, 0, 0);
    excHandlers[exc] = (void *) 0;
    return PL_INT_SUCCESS;
}
