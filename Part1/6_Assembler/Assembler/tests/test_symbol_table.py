import pytest
from Assembler.SymbolTable import SymbolTable


@pytest.fixture(scope="function")
def symbol_table():
    return SymbolTable()


def test_instantiation(symbol_table):
    assert symbol_table.get_table() == {}
