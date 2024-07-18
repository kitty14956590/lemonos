BUILD_DIR := build

CC := gcc
S := gcc
ASM := nasm
ASMFLAGS := -f elf32
CCFLAGS := -O3 -mgeneral-regs-only -mhard-float -static -m32 -mmmx -fallow-store-data-races -fno-builtin -fno-builtin-function -nostdlib -fno-defer-pop -fomit-frame-pointer -falign-functions=16 -funsafe-loop-optimizations -nostdlib -funsigned-char -Iincludes
LD := ld
LDFLAGS := -m elf_i386
MAKE := make
AR := ar
GRUB := grub-mkrescue
GRUBVOLID := "LEMON_OS"
GRUBDST := lemonos.iso
GRUBSRC := src/grub/*
QEMU := qemu-system-i386
QEMUFLAGS := -enable-kvm -cpu core2duo -smp 1 -serial stdio -serial vc -vga std
QEMUMEMORY := 128M

SOURCES=$(wildcard src/*.c)
ASM_SOURCES=$(wildcard src/*.asm)

OBJS=$(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
ASM_OBJS=$(patsubst src/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

OUTPUT := kernel.bin

default: mkdir build grub qemu

mkdir:
	mkdir -p ${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR} ${OUTPUT} ${GRUBDST}

grub:
	mkdir -p ${BUILD_DIR}/grub/
	mkdir -p ${BUILD_DIR}/grub/boot/
	mkdir -p ${BUILD_DIR}/grub/boot/grub/
	cp ${OUTPUT} ${BUILD_DIR}/grub/boot/kernel.bin
	cp -r ${GRUBSRC} ${BUILD_DIR}/grub/boot/grub/
	${GRUB} ${GRUBFLAGS} -o ${GRUBDST} ${BUILD_DIR}/grub/ -- -volid $(GRUBVOLID)

$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CCFLAGS) $^ -c -o $@

$(BUILD_DIR)/%.o: src/%.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

build: $(OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) -T src/link.ld -o $(OUTPUT) $^

qemu:
	$(QEMU) $(QEMUFLAGS) -cdrom $(GRUBDST) -m $(QEMUMEMORY)
