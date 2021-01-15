"""Parser is a class used to parse an assembly file.

"""
from Assembler.utils import remove_comment_from_instruction

class Parser:
    """A class used to read and parse an assembly file.

    Parameters
    ----------
    file_path: str
        A string representing the path to an assembly file

    Attributes
    ----------
    _file_path: str
        A string representing the path to the assembly file being parsed.
    _file: object
        Represents the file being parsed

    """
    def __init__(self, file_path):
        self._file_path = file_path
        self._file = None

    def get_file_path(self):
        """The path to the file being parsed.

        Returns
        -------
        str
            A string representing the path of the file being parsed.

        """
        return self._file_path

    def open_file(self):
        """Opens the assembly file to be parsed.

        Returns
        -------
        None

        """
        self._file = open(self._file_path, 'r')

    def get_next_assembly_line(self):
        """Retrieves the next assembly line from the file being parsed.

        Only the next valid assembly line corresponding to assembly code
        is retrieved. All empty lines or comment lines are skipped.

        Returns
        -------
        str
            A string corresponding to the next valid line of assembly
            code from the file being parsed.

        """
        while True:
            line = self._file.readline()
            if line == "":
                return None
            line = line.strip()
            if line and line != "" and line[:2] != "//":
                return remove_comment_from_instruction(line)

    def close_file(self):
        """Closes the assembly file.

        Returns
        -------
        None

        """
        if self._file:
            self._file.close()
            self._file = None
