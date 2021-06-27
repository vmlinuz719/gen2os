#if !defined (_PlProtection_)
#define _PlProtection_

/* PlProtection
 * Represents a CPU's memory protection facilities.
 * Only paging is to be exposed above the PL.
 */

class PlProtection {
public:
    /* Constructor should initialize the CPU's memory protection facilities.
     * For instance, on x86 this should set a valid GDT and store a pointer to
     * the initial identity map handed to us by BOOTBOOT.
     */
    PlInterrupt() { }
    virtual ~PlInterrupt() { }

    // The actual methods used to manage *physical* memory
    // are platform independent and do not appear here

    /* PlPgIdentityMap: Revert to initial identity map that we got from the bootloader
     * Returns: Physical address of current page map
     */
    virtual void *PlPgIdentityMap();

    /* Figure out what to do with this later idk */
};

#endif
