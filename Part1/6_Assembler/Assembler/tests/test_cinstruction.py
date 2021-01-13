import pytest
from Assembler.CInstruction import CInstruction


@pytest.fixture(scope="function")
def comp_m():
    return CInstruction("D+M", "D", "JMP")


@pytest.fixture(scope="function")
def comp_a():
    return CInstruction("A-D", "AM", "JGE")


@pytest.fixture(scope="function")
def no_dest():
    return CInstruction(comp="0", dest=None, jump="JLE")


@pytest.fixture(scope="function")
def no_jump():
    return CInstruction(comp="-D", dest="AMD", jump=None)


def test_instantiation(comp_m, comp_a, no_dest, no_jump):
    assert comp_m.get_comp_component() == "D+M"
    assert comp_m.get_dest_component() == "D"
    assert comp_m.get_jump_component() == "JMP"
    assert comp_a.get_comp_component() == "A-D"
    assert comp_a.get_dest_component() == "AM"
    assert comp_a.get_jump_component() == "JGE"
    assert no_dest.get_comp_component() == "0"
    assert no_dest.get_dest_component() is None
    assert no_dest.get_jump_component() == "JLE"
    assert no_jump.get_comp_component() == "-D"
    assert no_jump.get_dest_component() == "AMD"
    assert no_jump.get_jump_component() is None
