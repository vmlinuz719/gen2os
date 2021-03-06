CPP = x86_64-elf-g++
LD = x86_64-elf-ld
STRIP = x86_64-elf-strip
READELF = x86_64-elf-readelf
AS = x86_64-elf-gcc
CPPFLAGS = -Wall -fpic -ffreestanding -fno-use-cxa-atexit -fno-stack-protector -fno-exceptions -fno-rtti -nostdlib -fno-leading-underscore -I../include/
LDFLAGS = -L. -lgcc -nostdlib -T link.ld
STRIPFLAGS = -s -K mmio -K fb -K bootboot -K environment -K initstack

OBJS:=kernel.o heap.o gdt.o idt.o bootcon.o spinlock.o bitmap.o font.o kbdtest.o 8259.o
CRTI_OBJ= 
CRTBEGIN_OBJ:= 
CRTEND_OBJ:= 
CRTN_OBJ= 
OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)

all: KRNL8664.SYS

KRNL8664.SYS: kernel.cpp heap.cpp gdt.cpp idt.cpp 8259.cpp bootcon.cpp spinlock.cpp bitmap.cpp kbdtest.cpp
	$(CPP) $(CPPFLAGS) -mno-red-zone -c kernel.cpp -o kernel.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c heap.cpp -o heap.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c gdt.cpp -o gdt.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c idt.cpp -o idt.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c spinlock.cpp -o spinlock.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c bitmap.cpp -o bitmap.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c 8259.cpp -o 8259.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c bootcon.cpp -o bootcon.o
	$(CPP) $(CPPFLAGS) -mgeneral-regs-only -mno-red-zone -c kbdtest.cpp -o kbdtest.o
	$(LD) -r -b binary -o font.o font.psf
	$(LD) $(LDFLAGS) $(OBJS) -o KRNL8664.SYS
	$(STRIP) $(STRIPFLAGS) KRNL8664.SYS
	$(READELF) -hls KRNL8664.SYS > krnl8664.txt

INITRD: KRNL8664.SYS # deprecated
	find ./*.SYS | cpio -H crc -o | gzip > INITCPIO.SYS

clean:
	rm *.o *.txt *.SYS || true
