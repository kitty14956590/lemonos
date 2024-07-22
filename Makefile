BUILD_DIR := build
SMALL_BUILD_DIR := small_build

CC := gcc
S := gcc
ASM := nasm
ASMFLAGS := -f elf32
ASMSMALLFLAGS := -f elf32
CCFLAGS := -O3 -mgeneral-regs-only -mhard-float -static -m32 -mmmx -fno-builtin -fno-builtin-function -fno-defer-pop -fomit-frame-pointer -falign-functions=16 -nostdlib -funsigned-char -Iincludes
CCSMALLFLAGS := -Oz -mgeneral-regs-only -mhard-float -static -m32 -mmmx -fno-builtin -fno-builtin-function -fno-defer-pop -fomit-frame-pointer -falign-functions=16 -nostdlib -funsigned-char -Iincludes
LD := ld
LDFLAGS := -m elf_i386 --strip-all --discard-all --discard-locals
LDSMALLFLAGS := -m elf_i386 --strip-all --discard-all --discard-locals --strip-debug
MAKE := make
AR := ar
GRUB := grub-mkrescue
GRUBFLAGS := --compress=xz -iso-level 3
GRUBVOLID := "LEMON_OS"
GRUBDST := lemonos.iso
GRUBSRC := src/grub/*
QEMU := qemu-system-i386
QEMUFLAGS := --enable-kvm -smp 1 -serial file:serial.txt -vga std
QEMUIDE := -blockdev driver=file,node-name=f0,filename=disk.img -device floppy,drive=f0
QEMUDBGFLAGS := -gdb tcp::1234 -S
QEMUMEMORY := 128M

SOURCES=$(wildcard src/*.c)
ASM_SOURCES=$(wildcard src/*.asm)

OBJS=$(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
ASM_OBJS=$(patsubst src/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
SMALL_OBJS=$(patsubst src/%.c,$(SMALL_BUILD_DIR)/%.o,$(SOURCES))
SMALL_ASM_OBJS=$(patsubst src/%.asm,$(SMALL_BUILD_DIR)/%.o,$(ASM_SOURCES))
OUTPUT := kernel.bin
SMALL_OUTPUT := kernel.bin

default: mkdir build grub qemu
small: small_mkdir small_build small_grub qemu

mkdir:
	mkdir -p ${BUILD_DIR}

small_mkdir:
	mkdir -p ${SMALL_BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR} ${OUTPUT} ${GRUBDST}

small_clean:
	rm -rf ${SMALL_BUILD_DIR} ${SMALL_OUTPUT} ${GRUBDST}

grub:
	mkdir -p ${BUILD_DIR}/grub/
	mkdir -p ${BUILD_DIR}/grub/boot/
	mkdir -p ${BUILD_DIR}/grub/boot/grub/
	cp ${OUTPUT} ${BUILD_DIR}/grub/boot/kernel.bin
	cp -r ${GRUBSRC} ${BUILD_DIR}/grub/boot/grub/
	${GRUB} ${GRUBFLAGS} -o ${GRUBDST} ${BUILD_DIR}/grub/ -- -volid $(GRUBVOLID)

small_grub:
	mkdir -p ${SMALL_BUILD_DIR}/grub/
	mkdir -p ${SMALL_BUILD_DIR}/grub/boot/
	mkdir -p ${SMALL_BUILD_DIR}/grub/boot/grub/
	cp ${SMALL_OUTPUT} ${SMALL_BUILD_DIR}/grub/boot/kernel.bin
	cp -r ${GRUBSRC} ${SMALL_BUILD_DIR}/grub/boot/grub/
	${GRUB} ${GRUBFLAGS} -o ${GRUBDST} ${SMALL_BUILD_DIR}/grub/ -- -volid $(GRUBVOLID)

$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CCFLAGS) $^ -c -o $@

$(BUILD_DIR)/%.o: src/%.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

$(SMALL_BUILD_DIR)/%.o: src/%.c
	$(CC) $(CCSMALLFLAGS) $^ -c -o $@

$(SMALL_BUILD_DIR)/%.o: src/%.asm
	$(ASM) $(ASMSMALLFLAGS) $^ -o $@

build: $(OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) -T src/link.ld -o $(OUTPUT) $^

small_build: $(SMALL_OBJS) $(SMALL_ASM_OBJS)
	$(LD) $(LDFLAGS) -T src/link.ld -o $(SMALL_OUTPUT) $^

qemu:
	$(QEMU) $(QEMUFLAGS) -cdrom $(GRUBDST) -m $(QEMUMEMORY)

debug_qemu:
	$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) -cdrom $(GRUBDST) -m $(QEMUMEMORY)
