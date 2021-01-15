import pytest
from Assembler import exceptions as exc


def test_symbol_not_found():
    with pytest.raises(exc.SymbolNotFound) as symbol_exc:
        raise exc.SymbolNotFound("ASymbol")
    assert (str(symbol_exc.value)
            == "ASymbol is not present in the symbol table")
