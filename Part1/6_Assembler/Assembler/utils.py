"""A set of utility functions used by the assembler.

"""
import re


def remove_comment_from_instruction(instruction_str):
    """Removes any inline comments from `instruction_str`.

    Parameters
    ----------
    instruction_str: str
        A string representing the instruction to be processed.

    Returns
    -------
    str
        The string obtained from `instruction_str` after removing any
        inline comments.

    """
    comment_pos = instruction_str.find("//")
    if comment_pos == -1:
        return instruction_str.strip()
    return instruction_str[:comment_pos].strip()


def get_instruction_components(instruction_str):
    """Retrieves the components of `instruction_str`.

    Parameters
    ----------
    instruction_str: str
        A string representing the instruction to be processed.

    Returns
    -------
    list
        A list of strings containing the components of the instruction.

    """
    if instruction_str[0] == "@":
        return [instruction_str[1:]]
    return [command.strip().replace(" ", "")
            for command in re.split('[=;]', instruction_str)]


def fix_comp_component(comp_str):
    """Reorders `comp_str` to ensure a consistent ordering.

    To ensure correct conversion to a binary instruction, comp components
    must have a consistent ordering. Even though the operations `D+1`, `A+1`,
    'D+A', 'D&A', and 'D|A' are all commutative, this function ensures
    that they always appear in this order.

    Parameters
    ----------
    comp_str: str
        A string representing the computation component of a C-instruction.

    Returns
    -------
    str
        The string obtained from `comp_str` after ensuring a consistent
        ordering of the computation component for the C-instruction.

    """
    if len(comp_str) == 3:
        if ((comp_str[1] in ['+', '&', '|'] and comp_str[2] == 'D') or
            (comp_str[1] == '+' and comp_str[0] == '1')):
            return comp_str[2] + comp_str[1] + comp_str[0]
    return comp_str
