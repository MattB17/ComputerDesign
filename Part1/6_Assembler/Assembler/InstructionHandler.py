"""Implements an object used to handle the details of an assembly instruction.
This handler deals with the specific details of managing a particular
instruction.

"""
from Assembler.utils import get_instruction_components

class InstructionHandler:
    """Handles the details of interacting with an assembly instruction.

    Parameters
    ----------
    instruction_str: str
        A string representing a line from an assembly file.

    Attrbitues
    ----------
    _type: str
        Identifies the instruction type. Either "A" or "C"
    _components: list
        A list containing the components of the instruction. In the case of an
        A-instruction this list contains a single string referencing an address.
        For a C-instruction, this list has 2 or 3 elements containing the
        destination, command, and optionally a jump.

    """
    def __init__(self, instruction_str):
        self._type = "A" if instruction_str[0] == "@" else "C"
        self._components = get_instruction_components(instruction_str)

    def get_type(self):
        """The type of instruction.

        Returns
        -------
        str
            A string representing the instruction's type.

        """
        return self._type

    def get_components(self):
        """The components of the instruction.

        Returns
        -------
        list
            A list of strings representing the components of the instruction.

        """
        return self._components
