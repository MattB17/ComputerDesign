from Assembler.Parser import Parser

def test_instantiation():
    parser = Parser("an_assembly_file.asm")
    assert parser._file == "an_assembly_file.asm"
