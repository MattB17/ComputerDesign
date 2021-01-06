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
//    key = RAM[KBD]
//    color = 0
//    if key == 0 goto COLORSCREEN
//    color = -1
//    COLORSCREEN:
//      if addr >= 8192 + SCREEN goto LOOP
//      RAM[addr] = color
//      addr = addr + 1
//      goto COLORSCREEN
