import pytest
from unittest.mock import MagicMock
from Assembler.AInstruction import AInstruction


@pytest.fixture(scope="function")
def instruction1():
    return AInstruction(29)

@pytest.fixture(scope="function")
def instruction2():
    return AInstruction(1755)


def test_instantiation(instruction1, instruction2):
    assert instruction1.get_decimal_address() == 29
    assert instruction2.get_decimal_address() == 1755


def test_binary_address(instruction1, instruction2):
    assert instruction1.get_binary_address() == "11101"
    assert instruction2.get_binary_address() == "11011011011"


def test_get_binary_instruction(instruction1, instruction2):
    instruction1.get_binary_address = MagicMock(return_value="11101")
    instruction2.get_binary_address = MagicMock(return_value="11011011011")
    assert instruction1.get_binary_instruction() == "0000000000011101"
    assert instruction2.get_binary_instruction() == "0000011011011011"
    instruction1.get_binary_address.assert_called_once()
    instruction2.get_binary_address.assert_called_once()
