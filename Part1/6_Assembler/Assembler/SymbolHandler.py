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

    def add_label_to_symbol_table(self, label, line_number):
        """Adds the pair (`label`, `line_number`) to the symbol table.

        Parameters
        ----------
        label: str
            A string representing the label to be added.
        line_number: int
            An integer representing the line number to which the label
            points in the assembly file.

        Returns
        -------
        None

        Side Effect
        -----------
        Adds `label` and `line_number` as a symbol-value pair to the
        symbol table.

        """
        label_end = label.find(")")
        self._symbol_table.add_symbol(label[1:label_end], line_number)

    def _find_all_labels(self):
        """Finds all labels in the assembly file.

        The assembly file is parsed to find all labels. These labels are
        then added to the symbol table to enable the conversion to machine
        code.

        Returns
        -------
        None

        Side Effect
        -----------
        Adds all labels and their corresponding line number in the assembly
        file to the symbol table.

        """
        line_number = 0
        while True:
            label, lines_parsed = self._find_next_label()
            if label is None:
                return
            line_number += lines_parsed
            self.add_label_to_symbol_table(label, line_number)

    def parse_assembly_file_for_labels(self):
        """Parses the assembly file, looking for all labels.

        Returns
        -------
        None

        Side Effect
        -----------
        Adds all the labels and their corresponding line numbers in the
        assembly file to the symbol table.

        """
        self.open_file()
        self._find_all_labels()
        self.close_file()


    def close_file(self):
        """Closes the assembly file.

        Returns
        -------
        None

        """
        if self._file:
            self._file.close()
            self._file = None
