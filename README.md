# x86-64 asm (gnu/linux)


## syscalls

* syscall number: rax
* syscall args: rdi rsi rdx r10 r8 r9
* return value: rax

[syscall table](https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/arch/x86/syscalls/syscall_64.tbl#L9-L331)


### pause

    rax=34

* [syscall_64.tbl](https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/arch/x86/syscalls/syscall_64.tbl#L43)
* [signal.c](https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/kernel/signal.c#L3550-L3557)


### exit

    rax=60, rdi=0

* [syscall_64.tbl](https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/arch/x86/syscalls/syscall_64.tbl#L69)
* [exit.c](https://github.com/torvalds/linux/blob/567cfea99af61ef19da42f8491da98cf94a4d166/kernel/exit.c#L843-L846)


## encoding

C-string are null-terminated; avoid null bytes in opcodes for injection

### encode.sh

    ./encode.sh objfile

dumps objfile's instruction mnemonics as sequence of opcodes in hex


### nulls.sh

    ./nulls.sh objfile

searches objfile's instruction opcodes for null bytes and shows offending mnemonic


## utilities

### gdb

add to ~/.gdbinit:

    set disassembly-flavor intel

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


## License

Copyright © 2015 Daniel J. Hofmann

Distributed under the MIT License (MIT).
