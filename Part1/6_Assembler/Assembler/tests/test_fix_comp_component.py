from Assembler.utils import fix_comp_component


def test_with_simple_instruction():
    assert fix_comp_component("0") == "0"
    assert fix_comp_component("!D") == "!D"


def test_with_non_commutative_instruction():
    assert fix_comp_component("A-1") == "A-1"
    assert fix_comp_component("A-D") == "A-D"


def test_instruction_in_correct_order():
    assert fix_comp_component("D+1") == "D+1"
    assert fix_comp_component("D+M") == "D+M"
    assert fix_comp_component("D&A") == "D&A"
    assert fix_comp_component("D|A") == "D|A"


def test_instruction_in_wrong_order():
    assert fix_comp_component("1+D") == "D+1"
    assert fix_comp_component("M+D") == "D+M"
    assert fix_comp_component("A&D") == "D&A"
    assert fix_comp_component("A|D") == "D|A"
