// bootstrap code
@256
D=A
@SP
M=D

// push constant 5
@5
D=A

@SP
A=M
M=D
@SP
M=M+1

// push constant 7
@7
D=A

@SP
A=M
M=D
@SP
M=M+1

// lt
@SP
M=M-1
A=M
D=M

@SP
A=M-1
D=M-D

@LT_TRUE_0
D;JLT

@SP
A=M-1
M=0
@LT_END_0
0;JMP

(LT_TRUE_0)
@SP
A=M-1
M=-1

(LT_END_0)

// push constant 7
@7
D=A

@SP
A=M
M=D
@SP
M=M+1

// push constant 5
@5
D=A

@SP
A=M
M=D
@SP
M=M+1

// gt
@SP
M=M-1
A=M
D=M

@SP
A=M-1
D=M-D

@GT_TRUE_1
D;JGT

@SP
A=M-1
M=0
@GT_END_1
0;JMP

(GT_TRUE_1)
@SP
A=M-1
M=-1

(GT_END_1)

// push constant 6
@6
D=A

@SP
A=M
M=D
@SP
M=M+1

// push constant 6
@6
D=A

@SP
A=M
M=D
@SP
M=M+1

// eq
@SP
M=M-1
A=M
D=M

@SP
A=M-1
D=M-D

@EQ_TRUE_2
D;JEQ

@SP
A=M-1
M=0
@EQ_END_2
0;JMP

(EQ_TRUE_2)
@SP
A=M-1
M=-1

(EQ_END_2)

