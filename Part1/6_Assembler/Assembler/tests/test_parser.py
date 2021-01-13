import pytest
from unittest.mock import patch, mock_open, MagicMock
from Assembler.Parser import Parser


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
    assert parser.get_next_assembly_line() == "@18"
    assert parser.get_next_assembly_line() is None
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
    assert parser.get_next_assembly_line() == "@18 // a comment"
    assert parser.get_next_assembly_line() == "D=D+M"
    assert parser.get_next_assembly_line() == "@24"
    assert parser.get_next_assembly_line() == "0;JMP"
    assert parser.get_next_assembly_line() is None
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
