"""The Instruction class represents an abstract base class for an assembly
instruction.

"""


from abc import ABC, abstractmethod


class Instruction(ABC):
    """A generic assembly instruction.

    """
    @abstractmethod
    def get_binary_instruction(self):
        """Retrieves the 16-bit binary string representing the instruction.

        Returns
        -------
        str
            A 16-bit binary string representation of the instruction.

        """
        pass
