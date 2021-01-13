import pytest
from unittest.mock import patch, MagicMock
from Assembler.CInstruction import CInstruction


FIX_STR = "Assembler.CInstruction.fix_comp_component"


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


@pytest.fixture(scope="function")
def wrong_order():
    return CInstruction(comp="M&D", dest="MD", jump="JNE")


def test_instantiation(comp_m, comp_a, no_dest, no_jump, wrong_order):
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
    assert wrong_order.get_comp_component() == "M&D"
    assert wrong_order.get_dest_component() == "MD"
    assert wrong_order.get_jump_component() == "JNE"


def test_dest_to_binary_dest_is_null(no_dest):
    assert no_dest._dest_to_binary() == "000"


def test_dest_to_binary_one_dest(comp_m):
    assert comp_m._dest_to_binary() == "010"


def test_dest_to_binary_two_dests(comp_a, wrong_order):
    assert comp_a._dest_to_binary() == "101"
    assert wrong_order._dest_to_binary() == "011"


def test_dest_to_binary_all_dests(no_jump):
    assert no_jump._dest_to_binary() == "111"


def test_jump_to_binary_jump_is_null(no_jump):
    assert no_jump._jump_to_binary() == "000"


def test_jump_to_binary_jump_is_not_null(comp_m, comp_a, no_dest, wrong_order):
    assert comp_m._jump_to_binary() == "111"
    assert comp_a._jump_to_binary() == "011"
    assert no_dest._jump_to_binary() == "110"
    assert wrong_order._jump_to_binary() == "101"


def test_comp_to_binary_m_in_comp(comp_m):
    with patch(FIX_STR, return_value="D+A") as mock_fix:
        assert comp_m._comp_to_binary() == "1000010"
    mock_fix.assert_called_once_with("D+A")


def test_comp_to_binary_a_in_comp(comp_a):
    with patch(FIX_STR, return_value="A-D") as mock_fix:
        assert comp_a._comp_to_binary() == "0000111"
    mock_fix.assert_called_once_with("A-D")


def test_comp_to_binary_with_zero(no_dest):
    with patch(FIX_STR, return_value="0") as mock_fix:
        assert no_dest._comp_to_binary() == "0101010"
    mock_fix.assert_called_once_with("0")


def test_comp_to_binary_only_D(no_jump):
    with patch(FIX_STR, return_value="-D") as mock_fix:
        assert no_jump._comp_to_binary() == "0001111"
    mock_fix.assert_called_once_with("-D")


def test_comp_to_binary_wrong_order(wrong_order):
    with patch(FIX_STR, return_value="D&A") as mock_fix:
        assert wrong_order._comp_to_binary() == "1000000"
    mock_fix.assert_called_once_with("A&D")


def test_get_binary_instruction_m_comp(comp_m):
    comp_m._comp_to_binary = MagicMock(return_value="1000010")
    comp_m._dest_to_binary = MagicMock(return_value="010")
    comp_m._jump_to_binary = MagicMock(return_value="111")
    assert comp_m.get_binary_instruction() == "1111000010010111"
    comp_m._comp_to_binary.assert_called_once()
    comp_m._dest_to_binary.assert_called_once()
    comp_m._jump_to_binary.assert_called_once()


def test_get_binary_instruction_a_comp(comp_a):
    comp_a._comp_to_binary = MagicMock(return_value="0000111")
    comp_a._dest_to_binary = MagicMock(return_value="101")
    comp_a._jump_to_binary = MagicMock(return_value="011")
    assert comp_a.get_binary_instruction() == "1110000111101011"
    comp_a._comp_to_binary.assert_called_once()
    comp_a._dest_to_binary.assert_called_once()
    comp_a._jump_to_binary.assert_called_once()


def test_get_binary_instruction_null_dest(no_dest):
    no_dest._comp_to_binary = MagicMock(return_value="0101010")
    no_dest._dest_to_binary = MagicMock(return_value="000")
    no_dest._jump_to_binary = MagicMock(return_value="110")
    assert no_dest.get_binary_instruction() == "1110101010000110"
    no_dest._comp_to_binary.assert_called_once()
    no_dest._dest_to_binary.assert_called_once()
    no_dest._jump_to_binary.assert_called_once()


def test_get_binary_instruction_null_jump(no_jump):
    no_jump._comp_to_binary = MagicMock(return_value="0001111")
    no_jump._dest_to_binary = MagicMock(return_value="111")
    no_jump._jump_to_binary = MagicMock(return_value="000")
    assert no_jump.get_binary_instruction() == "1110001111111000"
    no_jump._comp_to_binary.assert_called_once()
    no_jump._dest_to_binary.assert_called_once()
    no_jump._jump_to_binary.assert_called_once()


def test_get_binary_instruction_wrong_order(wrong_order):
    wrong_order._comp_to_binary = MagicMock(return_value="1000000")
    wrong_order._dest_to_binary = MagicMock(return_value="011")
    wrong_order._jump_to_binary = MagicMock(return_value="101")
    assert wrong_order.get_binary_instruction() == "1111000000011101"
    wrong_order._comp_to_binary.assert_called_once()
    wrong_order._dest_to_binary.assert_called_once()
    wrong_order._jump_to_binary.assert_called_once()
