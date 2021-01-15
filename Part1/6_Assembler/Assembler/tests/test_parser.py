import pytest
from unittest.mock import patch, mock_open, MagicMock, call
from Assembler.Parser import Parser


REMOVE_STR = "Assembler.Parser.remove_comment_from_instruction"


@pytest.fixture(scope="function")
def parser():
    return Parser("an_assembly_file.asm")


def test_instantiation(parser):
    assert parser.get_file_path() == "an_assembly_file.asm"


def test_open(parser):
    assert parser._file is None
    with patch("builtins.open", mock_open(read_data="")) as assembly_file:
        parser.open_file()
    assert parser._file.readlines() == []
    assembly_file.assert_called_once_with("an_assembly_file.asm", "r")


def test_next_assembly_line_simple(parser):
    with patch("builtins.open", mock_open(read_data="@18\n")):
        parser.open_file()
    with patch(REMOVE_STR, return_value="@18") as mock_remover:
        assert parser.get_next_assembly_line() == "@18"
        assert parser.get_next_assembly_line() is None
    mock_remover.assert_called_once_with("@18")
    parser.close_file()


def test_next_assembly_line_complex(parser):
    file_text = ("// Some random preamble\n" +
                 " // Some more preamble\n" +
                 "\n" +
                 "@18 // a comment\n" +
                 "D=D+M\n" +
                 "\n" +
                 "@24\n" +
                 "0;JMP\n")
    with patch("builtins.open", mock_open(read_data=file_text)):
        parser.open_file()
    line1_unclean = "@18 // a comment"
    line1_clean = "@18"
    line2 = "D=D+M"
    line3 = "@24"
    line4 = "0;JMP"
    with patch(REMOVE_STR,
               side_effect=(line1_clean, line2, line3, line4)) as mock_remover:
        assert parser.get_next_assembly_line() == line1_clean
        assert parser.get_next_assembly_line() == line2
        assert parser.get_next_assembly_line() == line3
        assert parser.get_next_assembly_line() == line4
        assert parser.get_next_assembly_line() is None
    assert mock_remover.call_count == 4
    remove_calls = [call(line1_unclean), call(line2), call(line3), call(line4)]
    mock_remover.assert_has_calls(remove_calls)
    parser.close_file()


def test_close_on_unopened_file(parser):
    assert parser._file is None
    parser.close_file()
    assert parser._file is None


def test_close_on_open_file(parser):
    parser._file = MagicMock()
    mock_close = MagicMock(side_effect=None)
    parser._file.close = mock_close
    parser.close_file()
    mock_close.assert_called_once()
    assert parser._file is None
