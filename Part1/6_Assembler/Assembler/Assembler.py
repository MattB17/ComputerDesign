"""The Assembler that converts assembly code into machine code.

"""
from Assembler.Parser import Parser
from Assembler.InstructionHandler import InstructionHandler
from Assembler.SymbolHandler import SymbolHandler


class Assembler:
    """Converts an assembly file to machine code.

    Parameters
    ----------
    assembly_file: str
        The name of the assembly file to be assembled.

    Attrbitues
    ----------
    _parser: Parser
        A Parser used to parse the assembly file.
    _symbol_handler: SymbolHandler
        A SYmbolHandler used to convert symbols to addresses during assembly.
    _machine_file_path: str
        The name of the file containing the assembled machine code.
    _machine_file: object
        The machine file being written to. A value of None indicates
        that the file is not open.

    """
    def __init__(self, assembly_file):
        self._parser = Parser(assembly_file)
        self._symbol_handler = SymbolHandler(assembly_file)
        self._machine_file_path = assembly_file[:-4] + ".hack"
        self._machine_file = None

    def get_machine_file_path(self):
        """Retrieves the path to the machine file.

        Returns
        -------
        str
            A string representing the path to the machine file.

        """
        return self._machine_file_path

    def start_assembly(self):
        """Initiates the assembly process.

        Returns
        -------
        None

        Side Effect
        -----------
        Opens the assembly file for parsing and the hack file in order
        to write the assembled machine code.

        """
        self._parser.open_file()
        self._machine_file = open(self._machine_file_path, 'w')

    def finish_assembly(self):
        """Finishes the assembly process.

        Returns
        -------
        None

        """
        self._parser.close_file()
        if self._machine_file:
            self._machine_file.close()
            self._machine_file = None

    def execute_first_pass(self):
        """Executes the first pass of the assembly process.

        In the first pass, the symbol table is initialized. Then the assembly
        file is parsed once and all labels are added to the symbol table.
        This is done in preparation for the second pass to enable conversion
        to machine code.

        Returns
        -------
        None

        Side Effect
        -----------
        Initializes the symbol table and adds all labels in the assembly file
        to the symbol file.

        """
        self._symbol_handler.parse_assembly_file_for_labels()

    def assemble_next_instruction(self, assembly_line):
        """Assembles `assembly_line` to machine code.

        Parameters
        ----------
        assembly_line: str
            A string representing the assembly code to be assembled.

        Returns
        -------
        None

        Side Effect
        -----------
        Writes a 16-bit machine instruction to the machine file.

        """
        instruction = InstructionHandler(assembly_line).get_instruction()
        self._machine_file.write(instruction.get_binary_instruction() + "\n")

    def run_conversion(self):
        """Converts the assembly file to machine code.

        Returns
        -------
        None

        Side Effect
        -----------
        Writes the machine code corresponding to the assembled code to the
        machine file.

        """
        while True:
            assembly_line = self._parser.get_next_assembly_line()
            if assembly_line:
                self.assemble_next_instruction(assembly_line)
            else:
                return

    def assemble(self):
        """Converts the assembly file to machine code.

        Returns
        -------
        None

        Side Effect
        -----------
        Writes the assembled machine code to the machine file.

        """
        self.start_assembly()
        self.run_conversion()
        self.finish_assembly()
