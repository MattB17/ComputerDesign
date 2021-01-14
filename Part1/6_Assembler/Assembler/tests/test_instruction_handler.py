import pytest
from unittest.mock import patch, MagicMock
from Assembler.InstructionHandler import InstructionHandler


COMPONENTS_STR = "Assembler.InstructionHandler.get_instruction_components"
C_STR = "Assembler.InstructionHandler.CInstruction"


@pytest.fixture(scope="function")
def a_handler():
    instruction_str = "@18"
    with patch(COMPONENTS_STR, return_value=["18"]) as mock_components:
        handler = InstructionHandler(instruction_str)
    mock_components.assert_called_once_with(instruction_str)
    return handler


@pytest.fixture(scope="function")
def c_handler_full():
    instruction_str = "MD=D+1;JGT"
    components = ["MD", "D+1", "JGT"]
    with patch(COMPONENTS_STR, return_value=components) as mock_components:
        handler = InstructionHandler(instruction_str)
    mock_components.assert_called_once_with(instruction_str)
    return handler


@pytest.fixture(scope="function")
def c_handler_no_jump():
    instruction_str = "D=D&A"
    components = ["D", "D&A"]
    with patch(COMPONENTS_STR, return_value=components) as mock_components:
        handler = InstructionHandler(instruction_str)
    mock_components.assert_called_once_with(instruction_str)
    return handler


@pytest.fixture(scope="function")
def c_handler_no_dest():
    instruction_str = "A;JMP"
    components = ["A", "JMP"]
    with patch(COMPONENTS_STR, return_value=components) as mock_components:
        handler = InstructionHandler(instruction_str)
    mock_components.assert_called_once_with(instruction_str)
    return handler


@pytest.fixture(scope="function")
def c_handler_simple():
    with patch(COMPONENTS_STR, return_value=["M+1"]) as mock_components:
        handler = InstructionHandler("M+1")
    mock_components.assert_called_once_with("M+1")
    return handler


def test_instantiation_a_instruction(a_handler):
    assert a_handler.get_type() == "A"
    assert a_handler.get_components() == ["18"]


def test_instantiation_c_instruction_full(c_handler_full):
    assert c_handler_full.get_type() == "C"
    assert c_handler_full.get_components() == ["MD", "D+1", "JGT"]


def test_instantiation_c_instruction_no_jump(c_handler_no_jump):
    assert c_handler_no_jump.get_type() == "C"
    assert c_handler_no_jump.get_components() == ["D", "D&A"]


def test_instantiation_c_instruction_no_dest(c_handler_no_dest):
    assert c_handler_no_dest.get_type() == "C"
    assert c_handler_no_dest.get_components() == ["A", "JMP"]


def test_instantiation_c_instruction_simple(c_handler_simple):
    assert c_handler_simple.get_type() == "C"
    assert c_handler_simple.get_components() == ["M+1"]


@patch("Assembler.InstructionHandler.AInstruction")
def test_AInstruction(ainstruction, a_handler):
    mock_ainstruction = MagicMock()
    ainstruction.return_value = mock_ainstruction
    assert a_handler._AInstruction() == mock_ainstruction
    ainstruction.assert_called_once_with("18")


@patch(C_STR)
def test_CInstruction_full(cinstruction, c_handler_full):
    mock_cinstruction = MagicMock()
    cinstruction.return_value = mock_cinstruction
    assert c_handler_full._CInstruction() == mock_cinstruction
    cinstruction.assert_called_once_with(comp="D+1", dest="MD", jump="JGT")


@patch(C_STR)
def test_CInstruction_no_jump(cinstruction, c_handler_no_jump):
    mock_cinstruction = MagicMock()
    cinstruction.return_value = mock_cinstruction
    assert c_handler_no_jump._CInstruction() == mock_cinstruction
    cinstruction.assert_called_once_with(comp="D&A", dest="D")


@patch(C_STR)
def test_CInstruction_no_dest(cinstruction, c_handler_no_dest):
    mock_cinstruction = MagicMock()
    cinstruction.return_value = mock_cinstruction
    assert c_handler_no_dest._CInstruction() == mock_cinstruction
    cinstruction.assert_called_once_with(comp="A", jump="JMP")


@patch(C_STR)
def test_CInstruction_simple(cinstruction, c_handler_simple):
    mock_cinstruction = MagicMock()
    cinstruction.return_value = mock_cinstruction
    assert c_handler_simple._CInstruction() == mock_cinstruction
    cinstruction.assert_called_once_with(comp="M+1")


def test_get_instruction_a_instruction(a_handler):
    mock_a_instruction = MagicMock()
    a_handler._AInstruction = MagicMock(return_value=mock_a_instruction)
    assert a_handler.get_instruction() == mock_a_instruction
    a_handler._AInstruction.assert_called_once()


def test_get_instruction_c_instruction(c_handler_full):
    mock_c_instruction = MagicMock()
    c_handler_full._CInstruction = MagicMock(return_value=mock_c_instruction)
    assert c_handler_full.get_instruction() == mock_c_instruction
    c_handler_full._CInstruction.assert_called_once()
