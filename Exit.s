section .text
  global _start


_start:
  xor rax, rax
  mov al, 60
  xor rdi, rdi
  syscall


; vim: set tabstop=2 shiftwidth=2 expandtab:
