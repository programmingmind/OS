BIN_DIR=osx-toolchain/bin/
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra

myos.bin: linker.ld boot.o common.o keyboard.o gdt.o isr.o pic.o tables.o terminal.o timer.o kernel.o
	$(BIN_DIR)i686-pc-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o common.o keyboard.o gdt.o isr.o pic.o tables.o terminal.o timer.o kernel.o -lgcc

%.o: %.asm
	nasm -felf $< -o $@

%.o: %.c %.h
	$(BIN_DIR)i686-pc-elf-gcc -c $< -o $@ $(CFLAGS)

%.o: %.c
	$(BIN_DIR)i686-pc-elf-gcc -c $< -o $@ $(CFLAGS)

clean:
	rm -f boot.o common.o keyboard.o gdt.o isr.o pic.o tables.o terminal.o timer.o kernel.o myos.bin
