; db - 255
; dw - 65 535
; dd - 4 294 967 295
; dq - 18 446 744 073 709 551 615

; QCode Plus v. 0.0.8
default rel
; = = = = = = = = = = VARIABLES = = = = = = = = = =
section .data
	nl db 10
	nll equ $ - nl
	qcp_int_x dd 15 ; int x = 15;
	qcp_print_0_0 db "Hello, World!" ; print("Hello, World!");
	qcp_print_0_0_len equ $ - qcp_print_0_0 ; print("Hello, World!");

section .bss
	itt_bfr resb 12

; = = = = = = = = = = INT TO TEXT = = = = = = = = = =
section .text
	global _start
intToText:
	mov rcx, 0
	mov ebx, 10
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
; = = = = = = = = = = CODE = = = = = = = = = =
; int x = 15;
;
; func main() {
; 	int y = 7;
; 	int z = 2;
; 	x += y;
; 	x += z;
; 	if (x == 24) {
; 		print(v:x 10);
; 	}
; }
;
; func test() {
; 	print("Hello, World!");
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

	mov eax, [rel qcp_int_x] ; if (x == 24)
	cmp eax, 24              ; if (x == 24)
	jne .if_0_end            ; if (x == 24)

	mov eax, [rel qcp_int_x] ; print('v:x' 10);
	call intToText           ; print('v:x' 10);
	call prt                 ; print('v:x' 10);
	call prtln               ; print(v:x '10');

.if_0_end:
	; = = = END = = =
	add rsp, 16 ; int y = 7; | int z = 2;

	mov rax, 60
	mov rdi, 0
	syscall
	ret
_qcp_func_test:
	mov rsi, qcp_print_0_0     ; print("Hello, World!");
	mov rdx, qcp_print_0_0_len ; print("Hello, World!");
	call prt
	ret
