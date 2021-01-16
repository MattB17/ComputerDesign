import pytest
from unittest.mock import patch, mock_open, MagicMock, call
from Assembler.Parser import Parser


REMOVE_STR = "Assembler.Parser.remove_comment_from_instruction"
SYMBOL_STR = "Assembler.Parser.is_symbol_reference"


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
                 "(LOOP)\n" +
                 "@18 // a comment\n" +
                 "D=D+M\n" +
                 "\n" +
                 "@LOOP\n" +
                 "0;JMP\n")
    with patch("builtins.open", mock_open(read_data=file_text)):
        parser.open_file()
    line1_unclean = "@18 // a comment"
    line1_clean = "@18"
    line2 = "D=D+M"
    line3 = "@LOOP"
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


def test_next_symbol_line_no_symbol(parser):
    with patch("builtins.open", mock_open(read_data="@18\n")):
        parser.open_file()
    with patch(REMOVE_STR, return_value="@18") as mock_remover, \
        patch(SYMBOL_STR, return_value=False) as mock_symbol:
        assert parser.get_next_symbol_line() is None
    mock_remover.assert_called_once_with("@18")
    mock_symbol.assert_called_once_with("@18")
    parser.close_file()


def test_next_symbol_line_simple_variable(parser):
    with patch("builtins.open", mock_open(read_data="@var\n")):
        parser.open_file()
    with patch(REMOVE_STR, return_value="@var") as mock_remover, \
        patch(SYMBOL_STR, return_value=True) as mock_symbol:
        assert parser.get_next_symbol_line() == "@var"
        assert parser.get_next_symbol_line() is None
    mock_remover.assert_called_once_with("@var")
    mock_symbol.assert_called_once_with("@var")
    parser.close_file()


def test_next_symbol_line_simple_label(parser):
    with patch("builtins.open", mock_open(read_data="(LOOP)\n")):
        parser.open_file()
    with patch(REMOVE_STR, return_value="(LOOP)") as mock_remover, \
        patch(SYMBOL_STR, return_value=True) as mock_symbol:
        assert parser.get_next_symbol_line() == "(LOOP)"
        assert parser.get_next_symbol_line() is None
    mock_remover.assert_called_once_with("(LOOP)")
    mock_symbol.assert_not_called()
    parser.close_file()


def test_next_symbol_line_complex(parser):
    file_text = ("// Some random preamble\n" +
                 " // Some more preamble\n" +
                 "\n" +
                 "(LOOP)\n" +
                 "@var // a comment\n" +
                 "D=D+M\n" +
                 "\n" +
                 "@LOOP\n" +
                 "0;JMP\n")
    with patch("builtins.open", mock_open(read_data=file_text)):
        parser.open_file()
    symbol1 = "(LOOP)"
    symbol2_unclean = "@var // a comment"
    symbol2_clean = "@var"
    commanda = "D=D+M"
    symbol3 = "@LOOP"
    commandb = "0;JMP"
    with patch(REMOVE_STR,
               side_effect=(symbol1, symbol2_clean, commanda,
                            symbol3, commandb)) as mock_remover, \
        patch(SYMBOL_STR,
              side_effect=(True, False, True, False)) as mock_symbol:
        assert parser.get_next_symbol_line() == symbol1
        assert parser.get_next_symbol_line() == symbol2_clean
        assert parser.get_next_symbol_line() == symbol3
        assert parser.get_next_symbol_line() is None
    remove_calls = [call(symbol1), call(symbol2_unclean),
                    call(commanda), call(symbol3), call(commandb)]
    mock_remover.assert_has_calls(remove_calls)
    assert mock_remover.call_count == 5
    assert mock_symbol.call_count == 4
    symbol_calls = [call(symbol2_clean), call(commanda),
                    call(symbol3), call(commandb)]
    mock_symbol.assert_has_calls(symbol_calls)


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
