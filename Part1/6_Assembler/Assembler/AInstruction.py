"""A container class holding the contents of an A-instruction used to convert
to the binary equivalent instruction.

"""
from Assembler.Instruction import Instruction


class AInstruction(Instruction):
    """Holds the information related to an A-Instruction.

    Parameters
    ----------
    decimal_address: int
        An integer representing the address referred to by the A-instruction.

    Attrbitues
    ----------
    _address: int
        The address of the A-instruction in decimal form.

    """
    def __init__(self, decimal_address):
        self._address = decimal_address

    def get_decimal_address(self):
        """The decimal representation of the address.

        Returns
        -------
        int
            An integer which is the decimal representation of the address
            referred to by the A-instruction.

        """
        return self._address

    def get_binary_address(self):
        """The binary representation of the address.

        Returns
        -------
        str
            A string corresponding to the binary address refered to
            by the A-instruction.

        """
        return str(bin(self._address))[2:]

    def get_binary_instruction(self):
        """The binary representation of the A-instruction

        Returns
        -------
        str
            A string corresponding to the 16-bit binary representation
            of the A-instruction.

        """
        return self.get_binary_address().zfill(16)
