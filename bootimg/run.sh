#!/bin/bash

cp ../cpp/KRNL8664.SYS ./boot/
mkbootimg config.json gen2.iso
qemu-system-x86_64 -cdrom gen2.iso -smp 2 -d int
