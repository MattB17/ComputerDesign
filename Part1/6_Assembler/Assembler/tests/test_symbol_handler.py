import pytest
from unittest.mock import patch, MagicMock
from Assembler.SymbolHandler import SymbolHandler


TABLE_STR = "Assembler.SymbolHandler.SymbolTable"
ASSEMBLY_FILE = "/path/to/an/assembly/file.asm"


@pytest.fixture(scope="function")
def symbol_table():
    return MagicMock()


@pytest.fixture(scope="function")
def handler(symbol_table):
    with patch(TABLE_STR, return_value=symbol_table) as mock_table:
        handler = SymbolHandler(ASSEMBLY_FILE)
    mock_table.assert_called_once()
    return handler


def test_instantiation(symbol_table, handler):
    assert handler._file_path == ASSEMBLY_FILE
    assert handler._file is None
    assert handler._symbol_table == symbol_table
    assert handler._new_var_address == 16
