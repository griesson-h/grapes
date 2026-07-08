[BITS 16]
[ORG 0x7c00]

KERNEL equ 0x1000

start:
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov si, str
    call print

    ; a20
    in al, 0x92
    test al, 2
    jnz load_kernel
    or al, 2
    and al, 0xFE
    out 0x92, al
load_kernel:
    ; load the kernel
    mov ah, 0x42
    mov dl, 0x80
    mov si, DAP
    int 0x13
    js disk_error
    cmp ah, 0
    jnz disk_error
switch_pm:
    cli

    lgdt [gdt_descriptor]

    ; switch to pm
    mov eax, cr0
    or al, 1    ; THE BIT
    mov cr0, eax
    jmp 0x08:pm

print:
    lodsb
    cmp al, 0
    jz done
    mov ah, 0x0e
    int 0x10
    jmp print

done:
    ret

disk_error:
    mov si, disk_err_msg
    call print
    jmp $


[BITS 32]
pm:
    mov ax,10h
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    jmp KERNEL
    jmp $



DAP:
    db 0x10
    db 0x00
    dw 0x0032
    dd KERNEL
    dq 0x1


gdt_start:
gdt_null:
    dq 0
gdt_code:
    dw 0xffff, 0x0000
    db 0x00, 0x9a, 0xcf, 0x00
gdt_data:
    dw 0xffff, 0x0000
    db 0x00, 0x92, 0xcf, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


str: db 'Hello World!', 0
disk_err_msg: db 'Disk read error!', 0

times 510 - ($ - $$) db 0
dw 0xAA55
