default rel

%macro RT 2
	mov eax, %2
	mov [rel qcpi%1], eax
%endmacro
%macro qop 1
	pop rax
	mov [rel %1], rax
%endmacro
%macro qet 2
	mov rax, [rsp + (%1 * 8)]
	mov [rel qcpi%2], rax
%endmacro

%macro goto 2
	lea rax, [rel .b%1]
	mov [rel qcpf], rax
	push rax
	push qword [rel qcpqp]
	mov qword [rel qcpqp], 0
	jmp %2
.b%1:
%endmacro
%macro qpush 1
	add qword [rel qcpqp], 8
	push %1
%endmacro
%macro endf 0
	add rsp, [rel qcpqp]
	pop [rel qcpqp]
	pop [rel qcpf]
	jmp [rel qcpf]
%endmacro

; je ==
; jne !=
; jl <
; jle <=
; jg >
; jge >=
%macro if 3
	mov rax, %1
	mov rbx, %3
	cmp rax, rbx
	%ifidn %2, ==
		jne .siema
	%endif
%endmacro
%macro endif 0

%endmacro


section .data
	qcpia dq 0
	qcpib dq 0
	qcpqp dq 0
	qcpf dq 0
section .bss
	itt_bfr resb 21
	prtbh_bfr resb 64
section .rodata
	qcp_nl db 10
	qcp_str_Decimal_1_0 db "Decimal: "
	qcp_strl_Decimal_1_0 equ $ - qcp_str_Decimal_1_0

	qcp_str_Binary_0_28_3_1_0 db "Binary (8): "
	qcp_strl_Binary_0_28_3_1_0 equ $ - qcp_str_Binary_0_28_3_1_0

	qcp_str_HEX_0_24_3_1_0 db "HEX (4): "
	qcp_strl_HEX_0_24_3_1_0 equ $ - qcp_str_HEX_0_24_3_1_0

section .text
	global _start

; = = = INT TO TEXT = = =
intToText:
	mov rcx, 0
	mov ebx, 10
	xor rdx, rdx
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
; 	inc rdx
	ret
; = = = PRINT = = =
prt:
	mov rax, 1
	mov rdi, 1
	syscall
	ret
prtln:
	lea rsi, [rel qcp_nl]
	mov rdx, 1
	call prt
	ret
; = = = PRINT BINARY = = =
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
	lea rsi, [rel prtbh_bfr]
	mov rdx, [rel qcpib]
	call prt
	ret
; = = = PRINT HEX = = =
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
	lea rsi, [rel prtbh_bfr]
	mov rdx, [rel qcpib]
	call prt
	ret

; = = = INT TO TEXT = = =
	; RT a, [value]
	; call intToText
	; call prt
; = = = PRINT = = =
	; mov rsi, [text]
	; mov rdx, [text length]
	; call prt
; = = = PRINT BINARY = = =
	; RT a, [value]
	; RT b, [length]
	; call prtB
; = = = PRINT HEX = = =
	; RT a, [value]
	; RT b, [length]
	; call prtH
; a-z A-Z 0-9 _ .
_start:
	push 10 ; int x(10);
	goto 0, main

	mov rax, 60
	mov rdi, 0
	syscall
	ret

main:       ; func main() {
	qpush 26 ;     int num(26);
	; println("Decimal: ", v:num);
	mov rsi, qcp_str_Decimal_1_0
	mov rdx, qcp_strl_Decimal_1_0
	call prt
	qet 0, a
	call intToText
	call prt
	call prtln
	; println("Binary (8): ", b:num'8);
	mov rsi, qcp_str_Binary_0_28_3_1_0
	mov rdx, qcp_strl_Binary_0_28_3_1_0
	call prt
	; qet 0, a
	RT b, 8
	call prtB
	call prtln
	; test();
	goto 1, test
	; println("HEX (4): ", h:num'4);
	mov rsi, qcp_str_HEX_0_24_3_1_0
	mov rdx, qcp_strl_HEX_0_24_3_1_0
	call prt
	qet 0, a
	RT b, 4
	call prtH
	call prtln
	endf

test:
	qpush 6 ; int num(6);
	; if(num == 6) {
	;     println(v:num);
	; }
	if [rel qcpia] == 6
	; println(v:num);
	qet 0, a
	call intToText
	call prt
	call prtln
	endif

	endf
