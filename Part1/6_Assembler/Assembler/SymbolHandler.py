"""A class to handle all of the symbols in an assembly file. It maintains
a symbol table and has functionality to parse the assembly file to find labels
and add them to the symbol table. It also handles the conversion from a symbol
instruction to a valid a instruction.

"""
from Assembler.SymbolTable import SymbolTable


class SymbolHandler:
    """Handles the symbols in an assembly file.

    Parameters
    ----------
    assembly_file_path: str
        The path to the assembly file.

    Attributes
    ----------
    _file_path: str
        The path to the assembly file.
    _file: object
        The assembly file being parsed. A value of None indicates that the
        file is closed.
    _symbol_table: SymbolTable
        The SymbolTable containing the symbols of the assembly file.
    _new_var_address: int
        The next available address that can be used for a variable.

    """
    def __init__(self, assembly_file_path):
        self._file_path = assembly_file_path
        self._file = None
        self._symbol_table = SymbolTable()
        self._new_var_address = 16
