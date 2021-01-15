"""The SymbolTable class holds a symbol table mapping symbols to values. It
also handles the conversion from a symbol to an A-instruction.

"""


class SymbolTable:
    """Holds a table mapping symbols to values.

    Attrbitues
    ----------
    _table: dict
        A dictionary in which the keys are strings representing values
        and the values are strings representing the corresponding values.

    """
    def __init__(self):
        self._table = {}

    def get_table(self):
        """Retrieves the symbol table.

        Returns
        -------
        dict
            A dictionary representing the symbol table where keys are symbols
            and the values are the corresponding addresses.

        """
        return self._table
