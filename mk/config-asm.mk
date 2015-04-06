CC = gcc

AS = yasm
ASFLAGS = -f elf64 -g dwarf2 -Worphan-labels

LDFLAGS = -nostdlib
