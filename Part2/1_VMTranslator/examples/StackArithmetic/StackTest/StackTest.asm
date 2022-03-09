// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP0
D;JEQ
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP0)
@SP
M=M-1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP1
D;JEQ
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP1)
@SP
M=M-1
// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP2
D;JEQ
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP2)
@SP
M=M-1
// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP3
D;JLT
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP3)
@SP
M=M-1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP4
D;JLT
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP4)
@SP
M=M-1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
// lt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP5
D;JLT
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP5)
@SP
M=M-1
// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP6
D;JGT
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP6)
@SP
M=M-1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP7
D;JGT
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP7)
@SP
M=M-1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
// gt
@SP
A=M-1
D=M
A=A-1
D=M-D
M=-1
@CLEANUP8
D;JGT
@SP
A=M-1
A=A-1
M=M+1
(CLEANUP8)
@SP
M=M-1
// push constant 57
@57
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 31
@31
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 53
@53
D=A
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
A=M-1
D=M
A=A-1
D=D+M
M=D
@SP
M=M-1
// push constant 112
@112
D=A
@SP
A=M
M=D
@SP
M=M+1
// sub
@SP
A=M-1
D=M
A=A-1
D=M-D
M=D
@SP
M=M-1
// neg
@SP
A=M-1
M=-M
// and
@SP
A=M-1
D=M
A=A-1
D=D&M
M=D
@SP
M=M-1
// push constant 82
@82
D=A
@SP
A=M
M=D
@SP
M=M+1
// or
@SP
A=M-1
D=M
A=A-1
D=D|M
M=D
@SP
M=M-1
// not
@SP
A=M-1
M=!M
