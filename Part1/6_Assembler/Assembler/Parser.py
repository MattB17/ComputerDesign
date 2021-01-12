"""Parser is a class used to parse an assembly file.

"""

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
        with open(self._file_path, 'r') as assembly_file:
            self._file = assembly_file

    def close_file(self):
        """Closes the assembly file.

        Returns
        -------
        None

        """
        if self._file:
            self._file.close()
            self._file = None
