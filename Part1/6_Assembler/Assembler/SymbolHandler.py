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

    def is_register_instruction(self, instruction):
        """Checks if `instruction` corresponds to a register instruction.

        A register instruction has the form @R<num> where <num> is an integer
        in the range [0, 15]

        Returns
        -------
        bool
            True if `instruction` is a register instruction. Otherwise, False.

        """
        return ((instruction[0:2] == "@R") and
                (instruction[2:].isnumeric()) and
                (0 <= int(instruction[2:]) <= 15))

    def convert_register_instruction(self, register_instruction):
        """Converts `register_instruction` to an address instruction.

        An address instruction has the form @<num> where <num> is an
        integer referring to an actual physical address in RAM.

        Parameters
        ----------
        register_instruction: str
            A string representing a register instruction of the form @R<num>
            where <num> is an integer in the range [0, 15]

        Returns
        -------
        str
            A string representing the address instruction obtained from
            translating `register_instruction`.

        """
        return "@{}".format(register_instruction[2:])


    def ensure_symbol_in_table(self, symbol_instruction):
        """Ensures that the symbol of `symbol_instruction` is in the table.

        If the symbol associated with `symbol_instruction` already appears
        in the symbol table then it either corresponds to a label or a
        previously defined variable. Otherwise, it corresponds to a variable
        that has not been allocated.

        Parameters
        ----------
        symbol_instruction: str
            A string representing an A-instruction of the form @<var> where
            <var> is the name of a symbol.

        Returns
        -------
        None

        Side Effect
        -----------
        If the symbol associated with `symbol_instruction` is not in the
        symbol table then it is added to the table with an associated value
        equal to the next free variable address.

        """
        symbol = symbol_instruction[1:]
        if not self._symbol_table.has_symbol(symbol):
            self._symbol_table.add_symbol(symbol, self._new_var_address)
            self._new_var_address += 1

    def convert_to_address_instruction(self, symbol_instruction):
        """Converts `symbol_instruction` to an address instruction

        The conversion is done by substituting the symbol in
        `symbol_instruction` with the corresponding address from the
        symbol table.

        Parameters
        ----------
        symbol_instruction: str
            A string representing a symbol instruction having the form
            @<var> where <var> is a symbol

        Returns
        -------
        str
            A string representing an address instruction having the form
            @<num> where <num> is the value associated with the symbol from
            `symbol_instruction` found in the symbol table.

        """
        if self.is_register_instruction(symbol_instruction):
            return self.convert_register_instruction(symbol_instruction)
        self.ensure_symbol_in_table(symbol_instruction)
        return self._symbol_table.convert_to_address(symbol_instruction[1:])


    def close_file(self):
        """Closes the assembly file.

        Returns
        -------
        None

        """
        if self._file:
            self._file.close()
            self._file = None
