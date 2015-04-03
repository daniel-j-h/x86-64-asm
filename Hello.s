section .text
    global main

    main:
        mov rax, 60
        mov rdi, 42
        syscall

; vim: set tabstop=4 shiftwidth=4 expandtab:
