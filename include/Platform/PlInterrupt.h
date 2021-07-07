#if !defined (_PlInterrupt_)
#define _PlInterrupt_

/* PlInterrupt
 * Representation of an interrupt driven architecture.
 * Contains methods to handle software and hardware interrupts and exceptions
 * independently.
 */

#define PL_INT_SUCCESS                  0
#define PL_INT_QUERY_FAIL               0
#define PL_INT_IRQ_OUT_OF_RANGE         -1
#define PL_INT_IRQ_HANDLER_PRESENT      -2
#define PL_INT_IRQ_HANDLER_NOT_PRESENT  -3
#define PL_INT_EXC_OUT_OF_RANGE         -1
#define PL_INT_EXC_HANDLER_PRESENT      -2
#define PL_INT_EXC_HANDLER_NOT_PRESENT  -3

/* All supported exceptions
 * Registering a handler for any other exception is undefined behavior
 * Modelled off of the x86 architecture; ARM could easily emulate these
 * In fact x86 can probably dump these straight into the IDT
 */

#define PL_INT_DIVIDE_BY_ZERO           0
#define PL_INT_DEBUG                    1 // implementation dependent behavior
#define PL_INT_NMI                      2
#define PL_INT_BREAKPOINT               3
#define PL_INT_OVERFLOW                 4
#define PL_INT_BOUND_CHECK              5
#define PL_INT_ILLEGAL_INSTRUCTION      6
#define PL_INT_NO_FPU                   7
#define PL_INT_SYSCHK 8 // Standard behavior is to trigger a SYSCHK and halt
#define PL_INT_PROTECTION_FAULT         13
#define PL_INT_PAGE_FAULT               14
#define PL_INT_FPU_FAULT                16
#define PL_INT_ALIGNMENT_FAULT          17
#define PL_INT_SYSCHK2 18 // For particularly egregious system failures
#define MAX_EXC 19

class PlInterrupt {
public:
    /* Constructor should initialize the CPU's interrupt management system in
     * its entirety. Destructor behavior is undefined and implementation
     * dependent
     *
     * Syscalls will be handled with PlSyscall as they do not necessarily rely
     * on interrupts
     */

    /* PlIrqRegisterHandler: Register a hardware interrupt handler function
     *
     * int irq: Which IRQ to service
     * void *handlerFn: Pointer to the handler function
     *     You will probably need to cast from whatever the target system uses
     * bool allowNested: Hints that further interrupts should be allowed
     *     during handler execution
     * bool protect: Disallow interrupt from being called in user mode
     * Returns: 0 on success, -1 if IRQ out of range, -2 if IRQ handler
     *     already present
     */
    virtual int PlIrqRegisterHandler
        (int irq, void *handlerFn, bool allowNested, bool protect) = 0;

    /* PlIrqQueryHandler: See what handler is associated with an IRQ
     * int irq: IRQ to query
     * Returns: Handler address if present, 0 otherwise or on any error
     */
    virtual void *PlIrqQueryHandler(int irq) = 0;

    /* PlIrqFlushHandler: Invalidate an IRQ's handler (mask it first!)
     * int irq: IRQ to invalidate
     * Returns: 0 on success, -1 if IRQ out of range, -3 if IRQ handler not
     *     present
     */
    virtual int PlIrqFlushHandler(int irq) = 0;

    /* PlIrqMask and PlIrqUnmask: Mask or unmask an IRQ
     * Silently fail if IRQ is out of range
     */
    virtual void PlIrqMask(int irq) = 0;
    virtual void PlIrqMaskAll() = 0;
    virtual void PlIrqUnmask(int irq) = 0;

    /* PlIrqEOI: Inform the interrupt controller that we have serviced an
     * interrupt
     */
    virtual void PlIrqEOI(int irq) = 0;

    // Exception equivalents for the above methods (besides IRQ mask)

    virtual int PlExcRegisterHandler
        (int exc, void *handlerFn, bool allowNested, bool protect) = 0;
    virtual void *PlExcQueryHandler(int exc) = 0;
    virtual int PlExcFlushHandler(int exc) = 0;

    // Enable and disable interrupts

    virtual void PlIntDisable() = 0;
    virtual void PlIntEnable() = 0;
};

#endif
