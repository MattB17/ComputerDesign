from Assembler.utils import is_symbol_reference


def test_with_symbol_reference():
    assert is_symbol_reference("@var")


def test_with_c_instruction():
    assert not is_symbol_reference("D=D+M;JMP")


def test_with_a_instruction_no_symbol():
    assert not is_symbol_reference("@18")
