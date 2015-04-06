%include "Syscalls.mac"

section .text
  global _start


_start:
  xor rax, rax
  mov al, sys_pause
  syscall


; vim: set tabstop=2 shiftwidth=2 expandtab:
