import pytest
from Assembler.SymbolTable import SymbolTable
from Assembler import exceptions as exc
from Assembler.specs import HACK_SYMBOLS


@pytest.fixture(scope="function")
def symbol_table():
    return SymbolTable()


def test_instantiation(symbol_table):
    assert symbol_table.get_table() == HACK_SYMBOLS


def test_add_symbol(symbol_table):
    symbol_table.add_symbol("START", 12)
    symbol_table.add_symbol("var", 16)
    symbol_table.add_symbol("END", 27)
    assert symbol_table.get_table() == {**HACK_SYMBOLS,
                                        "START": 12,
                                        "var": 16,
                                        "END": 27}


def test_has_symbol(symbol_table):
    assert not symbol_table.has_symbol("START")
    symbol_table.add_symbol("START", 12)
    assert symbol_table.has_symbol("START")


def test_convert_to_address_symbol_in_table(symbol_table):
    symbol_table.add_symbol("START", 12)
    assert symbol_table.convert_to_address("START") == 12


def test_convert_to_address_symbol_not_in_table(symbol_table):
    with pytest.raises(exc.SymbolNotFound):
        symbol_table.convert_to_address("InvalidSymbol")
