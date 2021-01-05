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
