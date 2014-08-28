BIN_DIR=osx-toolchain/bin/

myos.bin: linker.ld boot.o kernel.o
	$(BIN_DIR)i686-pc-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

boot.o: boot.s
	$(BIN_DIR)i686-pc-elf-as boot.s -o boot.o

kernel.o: kernel.c
	$(BIN_DIR)i686-pc-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -f boot.o kernel.o myos.bin
