#if !defined (_PcuProtection_)
#define _PcuProtection_

#include <stdint.h>
#include <Platform/PCx64_Uniprocessor/gdt.h>
#include <Platform/PCx64_Uniprocessor/Protection.h>
#include <Platform/PlProtection.h>

/* PCx64_Uniprocessor/Protection.h
 * This class implements PlProtection for the x86_64 uniprocessor Platform
 * Layer. It initializes the GDT and provides an interface for working with the
 * x86's notoriously complex paging structures.
 */

class PcProtection : public PlProtection {
public:
    PcProtection();
    void *PlPgIdentityMap();
private:
    SegmentDescriptor gdt[8];
    uint64_t *initPML4;
};

#endif
