"""The Assembler that converts assembly code into machine code.

"""
from Assembler.Parser import Parser


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
    _machine_file: str
        The name of the file containing the assembled machine code.

    """
    def __init__(self, assembly_file):
        self._parser = Parser(assembly_file)
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
