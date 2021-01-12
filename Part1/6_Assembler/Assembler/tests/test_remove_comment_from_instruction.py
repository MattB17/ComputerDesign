from Assembler.utils import remove_comment_from_instruction


def test_with_no_comment():
    assert remove_comment_from_instruction("@18 ") == "@18"
    assert remove_comment_from_instruction("MD=D+1;JGT") == "MD=D+1;JGT"


def test_with_comment():
    assert remove_comment_from_instruction(
        "@18//references mem address 18") == "@18"
    assert remove_comment_from_instruction(
        "MD=D+1;JGT //Adds 1 to the value of the D register") == "MD=D+1;JGT"
