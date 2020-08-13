.text
.align 8
 
.globl saveRBP
saveRBP:
movq %rbp, %rax
ret

.globl saveRBX
saveRBX:
movq %rbx, %rax
ret

.globl saveR12
saveR12:
movq %r12, %rax
ret

.globl saveR13
saveR13:
movq %r13, %rax
ret

.globl saveR14
saveR14:
movq %r14, %rax
ret

.globl saveR15
saveR15:
movq %r15, %rax
ret


.globl setRBX
setRBX:
movq %rdi, %rbx
ret

.globl setR12
setR12:
movq %rdi, %r12
ret

.globl setR13
setR13:
movq %rdi, %r13
ret

.globl setR14
setR14:
movq %rdi, %r14
ret


.globl setR15
setR15:
movq %rdi, %r15
ret

.globl pareStack
pareStack:
movq %rdi, %rsp
movq %rsi, %rax
popq %rbp
ret
