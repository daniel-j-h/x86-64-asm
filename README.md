# x86-64 gnu/linux asm


## syscalls

* syscall number: rax
* syscall args:  rdi rsi rdx r10 r8 r9
* return value: rax

syscall table: https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/arch/x86/syscalls/syscall_64.tbl#L9-L331


### pause

https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/arch/x86/syscalls/syscall_64.tbl#L43
https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/kernel/signal.c#L3550-L3557


### exit

https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/arch/x86/syscalls/syscall_64.tbl#L69
https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/kernel/exit.c#L843-L846


## utilities

add to ~/.gdbinit:

    set disassembly-flavor intel


### gdb

registers:

    info registers

disassemble:

    disassemble _start

gui with register view:

    gdb -tui
    layout split
    tui reg general


### objdump

symbol table entries:

    objdump -t

assembler mnemonics

    objdump -d
