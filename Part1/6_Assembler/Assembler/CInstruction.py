"""A container class holding the contents of a C-instruction and used to
convert to the binary-equivalent instruction.

"""


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
