"""A collection of custom exceptions generated when dealing with the
Assembler.

"""


class SymbolNotFound(Exception):
    """An Exception when `symbol` is not present in the symbol table.

    Parameters
    ----------
    symbol: str
        The symbol that generated the exception.

    """
    def __init__(self, symbol):
        message = "{} is not present in the symbol table".format(symbol)
        Exception.__init__(self, message)
