// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Implementation by Matt Buckley

// Pseudo-Code
//    a = R0
//    b = R1
//    idx = 0
//    prod = 0
//  LOOP:
//    if idx >= b goto STOP
//    prod = prod + a
//    idx = idx + 1
//    goto LOOP
//  STOP:
//    R2 = prod

  // Load value of R0 (RAM[0]) into register D
  // then save that value into memory location referenced by variable a
  @R0
  D=M
  @a
  M=D
  // Load value of R1 (RAM[1]) into register D
  // then save that value into memory location referenced by variable b
  @R1
  D=M
  @b
  M=D
  // Set R2 to 0
  @R2
  M=0
  // Set memory location referenced by idx to value 0
  @idx
  M=0
  // Set memory location referenced by prod to value 0
  @prod
  M=0

(LOOP)
  // get current value of idx and subtract b from it
  // if the value is at least 0, end the loop
  @idx
  D=M
  @b
  D=D-M
  @STOP
  D;JGE
  // get the value of prod and add a to it
  // then put it back in the memory location of prod
  @prod
  D=M
  @a
  D=D+M
  @prod
  M=D
  // increment idx and return to top of loop
  @idx
  M=M+1
  @LOOP
  0;JMP

(STOP)
  // read value of prod and save it to memory location
  // referenced by R2 (RAM[2])
  @prod
  D=M
  @R2
  M=D

(END)
  // infinite loop to end program
  @END
  0;JMP
