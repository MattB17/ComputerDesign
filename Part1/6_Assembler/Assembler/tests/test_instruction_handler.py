from unittest.mock import patch
from Assembler.InstructionHandler import InstructionHandler


COMPONENTS_STR = "Assembler.InstructionHandler.get_instruction_components"


def test_instantiation_a_instruction():
    instruction_str = "@18"
    with patch(COMPONENTS_STR, return_value=["18"]) as mock_components:
        handler = InstructionHandler(instruction_str)
    assert handler.get_type() == "A"
    assert handler.get_components() == ["18"]
    mock_components.assert_called_once_with(instruction_str)


def test_instantiation_c_instruction():
    instruction_str = "MD=D+1;JGT"
    components = ["MD", "D+1", "JGT"]
    with patch(COMPONENTS_STR, return_value=components) as mock_components:
        handler = InstructionHandler(instruction_str)
    assert handler.get_type() == "C"
    assert handler.get_components() == components
    mock_components.assert_called_once_with(instruction_str)
