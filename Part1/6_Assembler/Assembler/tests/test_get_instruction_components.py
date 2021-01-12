from unittest.mock import patch
from Assembler.utils import get_instruction_components


REMOVE_STR = "Assembler.utils.remove_comment_from_instruction"


def test_with_a_instruction():
    cleaned_instruction = "@18"
    original_instruction = "@18 // sets A register to value 18"
    with patch(REMOVE_STR, return_value=cleaned_instruction) as mock_remove:
        components = get_instruction_components(original_instruction)
    mock_remove.assert_called_once_with(original_instruction)
    assert components == ["18"]


def test_c_instruction_no_space():
    instruction_str = "MD=D+1;JGT"
    with patch(REMOVE_STR, return_value=instruction_str) as mock_remove:
        components = get_instruction_components(instruction_str)
    mock_remove.assert_called_once_with(instruction_str)
    assert components == ["MD", "D+1", "JGT"]


def test_c_instruction_with_space():
    cleaned_instruction = "MD = D + 1 ; JGT"
    original_instruction = "MD = D + 1 ; JGT // Increments value of D register"
    with patch(REMOVE_STR, return_value=cleaned_instruction) as mock_remove:
        components = get_instruction_components(original_instruction)
    mock_remove.assert_called_once_with(original_instruction)
    assert components == ["MD", "D+1", "JGT"]


def test_c_instruction_no_jump():
    instruction_str = "MD = D + 1"
    with patch(REMOVE_STR, return_value=instruction_str) as mock_remove:
        components = get_instruction_components(instruction_str)
    mock_remove.assert_called_once_with(instruction_str)
    assert components == ["MD", "D+1"]
