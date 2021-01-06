// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed.
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Implementation by Matt Buckley

// Pseudo Code
//  LOOP:
//    addr = SCREEN
//    color = 0
//    if RAM[KBD] == 0 goto COLORSCREEN
//    color = -1
//    COLORSCREEN:
//      if addr >= 8192 + SCREEN goto LOOP
//      RAM[addr] = color
//      addr = addr + 1
//      goto COLORSCREEN

(LOOP)
  // get the screen address and save it to addr variable
  @SCREEN
  D=A
  @addr
  M=D

  // set color variable to 0 (default white)
  @color
  M=0

  // get key pressed on keyboard
  @KBD
  D=M

  // if no key pressed, don't update color
  @COLORSCREEN
  D;JEQ

  // otherwise set color to black
  @color
  M=-1

  // color the screen
  (COLORSCREEN)
    // check if addr >= 8192 + SCREEN
    // and if so jump back to top of loop
    @addr
    D=M
    @8192
    D=D-A
    @SCREEN
    D=D-A
    @LOOP
    D;JGE

    // save the current color into D
    @color
    D=M

    // recover the current address, go to that address
    // and set the register value to color
    @addr
    A=M
    M=D

    // increment addr
    @addr
    M=M+1

    // return to top of COLORSCREEN loop
    @COLORSCREEN
    0;JMP
