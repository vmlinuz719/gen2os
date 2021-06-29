#include <stdint.h>
#include <Platform/PCx64_Uniprocessor/gdt.h>
#include <Platform/PCx64_Uniprocessor/Protection.h>
#include <Platform/PlProtection.h>

/* PCx64_Uniprocessor/Protection.cpp
 * This class implements PlProtection for the x86_64 uniprocessor Platform
 * Layer. It initializes the GDT and provides an interface for working with the
 * x86's notoriously complex paging structures.
 */

// TODO: actually provide the paging interface

PcProtection::PcProtection() : PlProtection() {
    asm("cli");
    initGDT(gdt, 8);

    // uint64_t cr3 = 0;
    // asm("mov %%cr3,%0" : "=r"(cr3));
    // initPML4 = (uint64_t *) (cr3 & ~(0xFFFL));
}

void *PcProtection::PlPgIdentityMap() {
    uint64_t result = 0;
    asm("mov %%cr3,%0" : "=r"(result));
    asm("mov %0, %%cr3" :: "r"((uint64_t) initPML4));
    return (void *) result;
}
