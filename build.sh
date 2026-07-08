nasm -f bin boot.asm -o boot.bin
nasm -f elf32 entry.asm -o entry.o

i686-elf-gcc -o kernel.o -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -c kernel.c
i686-elf-gcc -o vga_text_drivers.o -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -nostdlib -nostartfiles -c vga_text_drivers.c
i686-elf-gcc -o basic_keyboard_driver.o -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -nostdlib -nostartfiles -c basic_keyboard_driver.c
i686-elf-gcc -o shell.o -m32 -ffreestanding -fno-builtin -fno-pie -no-pie -O2 -nostdlib -nostartfiles -c shell.c

i686-elf-ld -T linker.ld -m elf_i386 --no-warn-rwx-segments -o kernel.elf entry.o kernel.o vga_text_drivers.o basic_keyboard_driver.o shell.o
i686-elf-objcopy -O binary kernel.elf kernel.bin

dd if="/dev/zero" of="disk.img" bs=1M count=2
dd if="boot.bin" of="disk.img" bs=512 count=1 conv=notrunc
dd if="kernel.bin" of="disk.img" bs=512 seek=1 conv=notrunc

qemu-system-x86_64 -drive format=raw,file=disk.img
