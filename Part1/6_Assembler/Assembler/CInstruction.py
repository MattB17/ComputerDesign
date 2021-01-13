"""A container class holding the contents of a C-instruction and used to
convert to the binary-equivalent instruction.

"""
from Assembler import specs
from Assembler.utils import fix_comp_component


class CInstruction:
    """Holds the components of a C-instruction.

    A C-instruction has three components: a computation instruction, a
    destination instruction, and a jump instruction. All but the destination
    may be null.

    Parameters
    ----------
    comp: str
        A string representing the computation instruction.
    dest: str
        A string representing the destination instruction. The default value
        is `None`.
    jump: str
        A string representing the jump instruction. The default value is `None`.

    Attributes
    ----------
    _comp: str
        A string representing the computation instruction.
    _dest: str
        A string representing the destination instruction. A value of `None`
        indicates that there is no destination associated with the instruction.
    _jump: str
        A string representing the jump instruction. A value of `None` indicates
        that there is no jump associated with the the instruction.

    """
    def __init__(self, comp, dest=None, jump=None):
        self._comp = comp
        self._dest = dest
        self._jump = jump

    def get_comp_component(self):
        """The computation component for the C-instruction.

        Returns
        -------
        str
            A string representing the computation component of the
            C-instruction.

        """
        return self._comp

    def get_dest_component(self):
        """The destination component for the C-instruction.

        Returns
        -------
        str
            A string representing the destination component of the
            C-instruction.

        """
        return self._dest

    def get_jump_component(self):
        """The jump instruction for the C-instruction.

        Returns
        -------
        str
            A string representing the jump component of the
            C-instruction.

        """
        return self._jump

    def _dest_to_binary(self):
        """Converts the destination component to binary.

        Returns
        -------
        str
            A 3-element binary string representing the destination
            for the C-instruction.

        """
        if not self._dest:
            return "000"
        return (str(int(self._dest.find("A") > -1)) +
                str(int(self._dest.find("D") > -1)) +
                str(int(self._dest.find("M") > -1)))

    def _jump_to_binary(self):
        """Converts the jump component to binary.

        Returns
        -------
        str
            A 3-element binary string representing the jump condition
            for the C-instruction.

        """
        if not self._jump:
            return "000"
        return specs.JUMP_DICT[self._jump]

    def _comp_to_binary(self):
        """Converts the computation component to binary.

        Returns
        -------
        str
            A 7-element binary string representing the computation
            for the C-instruction.

        """
        a_bit = "0"
        m_pos = self._comp.find("M")
        comp_str = self._comp
        if m_pos > -1:
            a_bit = "1"
            comp_str = self._comp.replace("M", "A")
        return "{0}{1}".format(a_bit,
                               specs.COMP_DICT[fix_comp_component(comp_str)])

    def get_binary_instruction(self):
        """Retrieves the 16-bit binary representation of the instruction.

        Returns
        -------
        str
            The 16-bit binary string representation of the C-instruction.

        """
        return "111{0}{1}{2}".format(self._comp_to_binary(),
                                     self._dest_to_binary(),
                                     self._jump_to_binary())
