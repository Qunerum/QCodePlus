; db - 255 - 8B - 2H
; dw - 65 535 - 16B - 4H
; dd - 4 294 967 295 - 32B - 8H
; dq - 18 446 744 073 709 551 615 - 64B - 16H

; QCode Plus v. 0.0.8
default rel
; = = = = = = = = = = VARIABLES = = = = = = = = = =
section .data
	nl db 10
	nll equ $ - nl
	rtxt dd 0
	qcpia dd 0
	qcpib dd 0

	qcp_int_x dd 15 ; int x = 15;
	qcp_print_0 db "Hello, World!"
	qcp_print_0_len equ $ - qcp_print_0

	qcp_print_1 db "Binary: "
	qcp_print_1_len equ $ - qcp_print_1

; print("Hello, World!" 10 "Binary: " b:a'5 10 "HEX: 0x" h:a'3 10);

section .bss
	itt_bfr resb 21
	prtbh_bfr resb 64

; = = = = = = = = = = INT TO TEXT = = = = = = = = = =
; mov eax, [value]
; call intToText
; prt

; OUT: rsi(text) & rdx(text len)
section .text
	global _start
intToText:
	mov rcx, 0
	mov ebx, 10
	mov eax, [rel qcpia]
.ittLoop:
	mov edx, 0
	div ebx
	add edx, 48
	push rdx
	inc rcx
	cmp eax, 0
	jne .ittLoop
	lea rdi, [rel itt_bfr]
	mov rdx, rcx
	lea rsi, [rel itt_bfr]
.ittLoopWrite:
	pop rax
	mov [rdi], al
	inc rdi
	loop .ittLoopWrite
	inc rdx
	ret
; = = = = = = = = = = PRINT = = = = = = = = = =
; mov rsi, [Text]
; mov rdx, [Text Length]
; call prt
prt:
	mov rax, 1
	mov rdi, 1
	syscall
	ret
prtln:
	lea rsi, [rel nl]
	mov rdx, nll
	mov rax, 1
	mov rdi, 1
	syscall
	ret
; B - Binary | H - Hex
; = = = = = = = = = = PRINT BINARY = = = = = = = = = =
; RTA [value]
; RTB [length]
; call prtB
prtB:
	mov eax, [rel qcpia]
    lea rdi, [rel prtbh_bfr]
    mov rcx, 32
    sub rcx, [rel qcpib]
    shl eax, cl
    mov rcx, [rel qcpib]
    lea rdi, [rel prtbh_bfr]
.prtBloop:
    mov edx, eax
    shr edx, 31
    add dl, '0'
    mov [rdi], dl
    inc rdi
    shl eax, 1
    loop .prtBloop
    mov rsi, prtbh_bfr
    mov rdx, [rel qcpib]
    call prt
    ret
; = = = = = = = = = = PRINT HEX = = = = = = = = = =
; RTA [value]
; RTB [length]
; call prtH
prtH:
	mov eax, [rel qcpia]
    lea rdi, [rel prtbh_bfr]
    mov rcx, 8
    sub rcx, [rel qcpib]
    shl rcx, 2
    shl eax, cl
    mov rcx, [rel qcpib]
.prtHloop:
    rol eax, 4
    mov edx, eax
    and edx, 0xF
    cmp dl, 9
    jbe .prtHdigit
    add dl, 7
.prtHdigit:
    add dl, '0'
    mov [rdi], dl
    inc rdi
    loop .prtHloop

    mov rsi, prtbh_bfr
    mov rdx, [rel qcpib]
    call prt
    ret
%macro RTA 1
	mov eax, %1
	mov [rel qcpia], eax
%endmacro
%macro RTB 1
	mov eax, %1
	mov [rel qcpib], eax
%endmacro
; = = = = = = = = = = CODE = = = = = = = = = =
; int x = 15;
;
; func main() {
; 	int y = 7;
; 	int z = 2;
; 	x += y;
; 	x += z;
; 	test();
; }
;
; func test() {
; 	int a = 13;
; 	print("Hello, World!" 10 "Binary: " b:a'5 10 "HEX: 0x" h:a'3 10);
; 	// Hello, World!
; 	// Binary: 01101
; 	// HEX: 0x00D
; }

_start:
	sub rsp, 16 ; int y = 7; | int z = 2;

	mov dword [rsp], 7 ; int y = 7;
	mov dword [rsp+8], 2 ; int z = 2;

	mov eax, [rsp]                  ; x += y;
	add dword [rel qcp_int_x], eax  ; x += y;

	mov eax, [rsp+8]               ; x += z;
	add dword [rel qcp_int_x], eax ; x += z;

	call _qcp_func_test ; test();

	add rsp, 16 ; int y = 7; | int z = 2;

	mov rax, 60
	mov rdi, 0
	syscall
	ret
_qcp_func_test:
	sub rsp, 8 ; int a = 13;
	mov dword [rsp], 13

	mov rsi, qcp_print_0
	mov rdx, qcp_print_0_len
	call prt
	call prtln

	RTA [rsp]
	call intToText
	call prt
	call prtln

	RTA [rsp]
	RTB 5
	call prtB
	call prtln

	RTA [rsp]
	RTB 2
	call prtH
	call prtln

	add rsp, 8
	ret
