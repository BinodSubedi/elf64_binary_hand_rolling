section .data
  text db "hello!",10


section .text
  global _start

  _start:
    mov rax, 1
    mov rdi, 1
    mov rsi, text
    mov rdx, 7
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
