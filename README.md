# gen2os
Gen/2 Operating System for x86-64

I finally started making another OS, this time the right way, and in C++.

BUILDING IS NON-TRIVIAL. First you need a GCC cross compiler for x86_64-elf and libgcc needs to be compiled without red zone support. You need to symlink libgcc.a (no-red-zone) into cpp unless you have your environment configured right somehow. If you type `make` in the cpp folder and all goes well, you will have an INITRD image. This needs to be booted by a BOOTBOOT-compliant loader, either from BIOS, UEFI or GRUB. Good luck.

So far it can recognize keypresses. I need to make a physical memory allocator (and rewrite `malloc` to align allocations properly, grumble grumble).
