from Assembler import Assembler
import sys


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Please specify a file name")
    else:
        assembler = Assembler(str(sys.argv[1]))
        assembler.assemble()
