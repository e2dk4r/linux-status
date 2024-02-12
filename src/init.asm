%define SYS_EXIT 60

extern main

section .text
global _start

_start:
  ;xor rbp, rbp
  ;pop rdi      ; rdi = argc
  ;mov rsi, rsp ; rsi = argv
  ;and rsp, -16
  call main

  ; exit()
  mov rdi, rax
  mov rax, SYS_EXIT
  syscall
  ret

global syscall1
syscall1:
  mov rax, rdi
  mov rdi, rsi
  syscall
  ret

global syscall2
syscall2:
  mov rax, rdi
  mov rdi, rsi
  mov rsi, rdx
  syscall
  ret

global syscall3
syscall3:
  mov rax, rdi
  mov rdi, rsi
  mov rsi, rdx
  mov rdx, rcx
  syscall
  ret

global syscall4
syscall4:
  mov rax, rdi
  mov rdi, rsi
  mov rsi, rdx
  mov rdx, rcx
  mov r10, r8
  syscall
  ret

global syscall5
syscall5:
  mov rax, rdi
  mov rdi, rsi
  mov rsi, rdx
  mov rdx, rcx
  mov r10, r8
  mov r8, r9
  syscall
  ret
