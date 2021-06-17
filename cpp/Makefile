#
# mykernel/cpp/Makefile
#
# Copyright (C) 2017 - 2021 bzt (bztsrc@gitlab)
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
# This file is part of the BOOTBOOT Protocol package.
# @brief An example Makefile for sample kernel
#
#

CPP = x86_64-elf-g++
LD = x86_64-elf-ld
STRIP = x86_64-elf-strip
READELF = x86_64-elf-readelf
AS = x86_64-elf-gcc
CPPFLAGS = -Wall -fpic -ffreestanding -fno-use-cxa-atexit -fno-stack-protector -fno-exceptions -fno-rtti -nostdlib -fno-leading-underscore -I../include/
LDFLAGS = -L. -lgcc -nostdlib -T link.ld
STRIPFLAGS = -s -K mmio -K fb -K bootboot -K environment -K initstack

OBJS:=kernel.o heap.o gdt.o idt.o 8259.o bootcon.o kbdtest.o font.o
CRTI_OBJ= 
CRTBEGIN_OBJ:= 
CRTEND_OBJ:= 
CRTN_OBJ= 
OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)

all: INITRD

KRNL8664.SYS: kernel.cpp heap.cpp gdt.cpp idt.cpp 8259.cpp bootcon.cpp kbdtest.cpp
	$(CPP) $(CPPFLAGS) -mno-red-zone -c kernel.cpp -o kernel.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c heap.cpp -o heap.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c gdt.cpp -o gdt.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c idt.cpp -o idt.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c 8259.cpp -o 8259.o
	$(CPP) $(CPPFLAGS) -mno-red-zone -c bootcon.cpp -o bootcon.o
	$(CPP) $(CPPFLAGS) -mgeneral-regs-only -mno-red-zone -c kbdtest.cpp -o kbdtest.o
	$(LD) -r -b binary -o font.o font.psf
	$(LD) $(LDFLAGS) $(OBJS) -o KRNL8664.SYS
	$(STRIP) $(STRIPFLAGS) KRNL8664.SYS
	$(READELF) -hls KRNL8664.SYS > krnl8664.txt

INITRD: KRNL8664.SYS
	find ./*.SYS | cpio -H crc -o | gzip > INITCPIO.SYS

clean:
	rm *.o *.txt *.SYS || true
