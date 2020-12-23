// This file is a helper function
// as part of www.nand2tetris.org
// by Nisan and Schocken, MIT Press.
// File name: Dup16.tst

load Dup16.hdl,
output-file Dup16.out,
compare-to Dup16.cmp,
output-list in%B3.1.3 out%B1.16.1;

set in 0,
eval,
output;

set in 1,
eval,
output;
