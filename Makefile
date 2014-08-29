BIN_DIR=osx-toolchain/bin/

myos.bin: linker.ld boot.o gdt.o isr.o pic.o kernel.o
	$(BIN_DIR)i686-pc-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o gdt.o isr.o pic.o kernel.o -lgcc


%.o: %.asm
	nasm -felf $< -o $@

%.o: %.c
	$(BIN_DIR)i686-pc-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -f boot.o gdt.o isr.o pic.o kernel.o myos.bin
