from unittest.mock import patch
from Assembler.utils import get_instruction_components


def test_with_a_instruction():
    instruction_str = "@18"
    assert get_instruction_components(instruction_str) == ["18"]


def test_c_instruction_no_space():
    instruction_str = "MD=D+1;JGT"
    assert get_instruction_components(instruction_str) == ["MD", "D+1", "JGT"]


def test_c_instruction_with_space():
    instruction_str = "MD = D + 1 ; JGT"
    assert (get_instruction_components(instruction_str)
            == ["MD", "D+1", "JGT"])


def test_c_instruction_no_jump():
    instruction_str = "MD = D + 1"
    assert get_instruction_components(instruction_str) == ["MD", "D+1"]
