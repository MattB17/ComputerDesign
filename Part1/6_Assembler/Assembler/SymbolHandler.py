"""A class to handle all of the symbols in an assembly file. It maintains
a symbol table and has functionality to parse the assembly file to find labels
and add them to the symbol table. It also handles the conversion from a symbol
instruction to a valid a instruction.

"""
from Assembler.SymbolTable import SymbolTable
from Assembler.utils import remove_comment_from_instruction


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

    def open_file(self):
        """Opens the assembly file to be parsed.

        Returns
        -------
        None

        """
        self._file = open(self._file_path, 'r')

    def _find_next_label(self):
        """Finds the next label in the assembly file.

        Identifies the next point in the file having a label of the form
        (LABEL).

        Returns
        -------
        str, int
            A string representing the next label found, where a value of None
            indicates that no label was found in the rest of the file. The
            integer represents the number of assembly lines parsed while
            searching for the label.

        """
        lines_parsed = 0
        while True:
            line = self._file.readline()
            if line == "":
                return None, lines_parsed
            line = line.strip()
            if line and line != "" and line[0] == "(":
                return remove_comment_from_instruction(line), lines_parsed
            if line and line != "" and line[0:2] != "//":
                lines_parsed += 1


    def close_file(self):
        """Closes the assembly file.

        Returns
        -------
        None

        """
        if self._file:
            self._file.close()
            self._file = None
