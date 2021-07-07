#if !defined (_PcInterrupt_)
#define _PcInterrupt_

#include <stdint.h>
#include <Platform/PCx64_Uniprocessor/idt.h>
#include <Platform/PCx64_Uniprocessor/8259.h>
#include <Platform/PlInterrupt.h>

/* PCx64_Uniprocessor/Interrupt.h
 * This class implements PlInterrupt for the x86_64 uniprocessor Platform
 * Layer. It initializes the IDT and provides a (restricted) interface for
 * working with the x86's interrupt facilities.
 */

class PcInterrupt : public PlInterrupt {
public:
    // use any two numbers to remap the PIC
    PcInterrupt(uint8_t io0, uint8_t io1);

    int PlIrqRegisterHandler
        (int irq, void *handlerFn, bool allowNested, bool protect);
    void *PlIrqQueryHandler(int irq);
    int PlIrqFlushHandler(int irq);
    void PlIrqMask(int irq);
    void PlIrqMaskAll();
    void PlIrqUnmask(int irq);
    void PlIrqEOI(int irq);

    int PlExcRegisterHandler
        (int exc, void *handlerFn, bool allowNested, bool protect);
    void *PlExcQueryHandler(int exc);
    int PlExcFlushHandler(int exc);

    void PlIntDisable();
    void PlIntEnable();

private:
    Pc8259 pic;
    PcIDT idt;

    uint8_t irqOffset0, irqOffset1;

    void *irqHandlers[MAX_IRQ];
    void *excHandlers[MAX_EXC];
};

#endif
