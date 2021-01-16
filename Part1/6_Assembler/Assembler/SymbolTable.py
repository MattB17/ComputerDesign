"""The SymbolTable class holds a symbol table mapping symbols to values. It
also handles the conversion from a symbol to an A-instruction.

"""
import copy
from Assembler import exceptions as exc
from Assembler import specs


class SymbolTable:
    """Holds a table mapping symbols to values.

    Attrbitues
    ----------
    _table: dict
        A dictionary in which the keys are strings representing values
        and the values are integers representing the corresponding address.

    """
    def __init__(self):
        self._table = copy.deepcopy(specs.HACK_SYMBOLS)

    def get_table(self):
        """Retrieves the symbol table.

        Returns
        -------
        dict
            A dictionary representing the symbol table where keys are symbols
            and the values are the corresponding addresses.

        """
        return self._table

    def add_symbol(self, symbol, value):
        """Adds the `symbol` and `value` as a key-value pair to the table.

        Parameters
        ----------
        symbol: str
            A string representing the symbol being added to the table.
        value: int
            An integer representing the address associated with `symbol`

        Returns
        -------
        None

        """
        self._table[symbol] = value

    def has_symbol(self, symbol):
        """Determines if `symbol` is in the table.

        Parameters
        ----------
        symbol: str
            A string representing the symbol being queried.

        Returns
        -------
        bool
            True if there is an entry for `symbol` in the table.
            Otherwise, False.

        """
        return symbol in self._table

    def convert_to_address(self, symbol):
        """Converts symbol to an address.

        Parameters
        ----------
        symbol: str
            A string representing the symbol to be converted.

        Returns
        -------
        int
            An integer representing the address associated with `symbol`.

        """
        if self.has_symbol(symbol):
            return self._table[symbol]
        else:
            raise exc.SymbolNotFound(symbol)
