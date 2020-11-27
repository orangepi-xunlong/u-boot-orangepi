
.globl RMR_TO64

RMR_TO64:
       MRC p15,0,r1,c12,c0,2
       ORR r1,r1,#(0x3<<0)
       MCR p15,0,r1,c12,c0,2

Loop1:
       mov r0, #0
       mcr p15, 0, r0, c7, c0, 4
       B       Loop1
       BX      lr

