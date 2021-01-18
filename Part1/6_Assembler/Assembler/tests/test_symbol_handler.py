import pytest
from unittest.mock import patch, MagicMock, mock_open, call
from Assembler.SymbolHandler import SymbolHandler


TABLE_STR = "Assembler.SymbolHandler.SymbolTable"
ASSEMBLY_FILE = "/path/to/an/assembly/file.asm"
REMOVE_STR = "Assembler.SymbolHandler.remove_comment_from_instruction"


@pytest.fixture(scope="function")
def symbol_table():
    table = MagicMock()
    table.add_symbol = MagicMock(side_effect=None)
    return table


@pytest.fixture(scope="function")
def handler(symbol_table):
    with patch(TABLE_STR, return_value=symbol_table) as mock_table:
        handler = SymbolHandler(ASSEMBLY_FILE)
    mock_table.assert_called_once()
    return handler


def test_instantiation(symbol_table, handler):
    assert handler._file_path == ASSEMBLY_FILE
    assert handler._file is None
    assert handler._symbol_table == symbol_table
    assert handler._new_var_address == 16


def test_open(handler):
    assert handler._file is None
    with patch("builtins.open", mock_open(read_data="")) as assembly_file:
        handler.open_file()
    assert handler._file.readlines() == []
    assembly_file.assert_called_once_with(ASSEMBLY_FILE, 'r')


def test_find_next_label_empty_file(handler):
    with patch("builtins.open", mock_open(read_data="")):
        handler.open_file()
    with patch(REMOVE_STR) as mock_remover:
        assert handler._find_next_label() == (None, 0)
    mock_remover.assert_not_called()
    handler.close_file()


def test_find_next_label_simple_has_label(handler):
    with patch("builtins.open", mock_open(read_data="(LABEL)\n")):
        handler.open_file()
    label = "(LABEL)"
    with patch(REMOVE_STR, return_value=label) as mock_remover:
        assert handler._find_next_label() == (label, 0)
        assert handler._find_next_label() == (None, 0)
    mock_remover.assert_called_once_with(label)
    handler.close_file()


def test_find_next_label_simple_no_label(handler):
    with patch("builtins.open", mock_open(read_data="@18\n")):
        handler.open_file()
    with patch(REMOVE_STR) as mock_remover:
        assert handler._find_next_label() == (None, 1)
    mock_remover.assert_not_called()
    handler.close_file()


def test_find_next_label_complex(handler):
    file_text = ("// Some random preamble\n" +
                 "// Plus another comment\n" +
                 "\n" +
                 "(INITIAL) // Base case\n" +
                 "@R0\n" +
                 "D=D+M\n" +
                 "\n" +
                 "@LOOP\n" +
                 "0;JMP\n" +
                 "\n" +
                 "(LOOP)\n" +
                 "@LOOP\n" +
                 "0;JMP\n")
    with patch("builtins.open", mock_open(read_data=file_text)):
        handler.open_file()
    with patch(REMOVE_STR,
               side_effect=("(INITIAL)", "(LOOP)")) as mock_remover:
        assert handler._find_next_label() == ("(INITIAL)", 0)
        assert handler._find_next_label() == ("(LOOP)", 4)
        assert handler._find_next_label() == (None, 2)
    assert mock_remover.call_count == 2
    remove_calls = [call("(INITIAL) // Base case"), call("(LOOP)")]
    mock_remover.assert_has_calls(remove_calls)
    handler.close_file()


def test_add_label_to_symbol_table_simple(handler, symbol_table):
    handler.add_label_to_symbol_table("(LABEL)", 10)
    symbol_table.add_symbol.assert_called_once_with("LABEL", 10)


def test_add_label_to_symbol_table_complex(handler, symbol_table):
    handler.add_label_to_symbol_table("(DIFFERENT_LABEL)", 21)
    symbol_table.add_symbol.assert_called_once_with("DIFFERENT_LABEL", 21)


def test_find_all_labels_no_labels(handler):
    handler.add_label_to_symbol_table = MagicMock()
    handler._find_next_label = MagicMock(return_value=(None, 1))
    handler._find_all_labels()
    handler._find_next_label.assert_called_once()
    handler.add_label_to_symbol_table.assert_not_called()


def test_find_all_labels_one_label(handler):
    handler.add_label_to_symbol_table = MagicMock(side_effect=None)
    handler._find_next_label = MagicMock(
        side_effect=(("(LABEL)", 0), (None, 0)))
    handler._find_all_labels()
    assert handler._find_next_label.call_count == 2
    handler.add_label_to_symbol_table.assert_called_once_with("(LABEL)", 0)


def test_find_all_labels_multi_label(handler):
    handler.add_label_to_symbol_table = MagicMock(side_effect=None)
    handler._find_next_label = MagicMock(
        side_effect=(("(INITIAL)", 0), ("(LOOP)", 4),
                     ("(END)", 20), (None, 2)))
    handler._find_all_labels()
    assert handler._find_next_label.call_count == 4
    add_calls = [call("(INITIAL)", 0), call("(LOOP)", 4), call("(END)", 24)]
    handler.add_label_to_symbol_table.assert_has_calls(add_calls)
    assert handler.add_label_to_symbol_table.call_count == 3


def test_parse_assembly_file_for_labels(handler):
    handler.open_file = MagicMock(side_effect=None)
    handler._find_all_labels = MagicMock(side_effect=None)
    handler.close_file = MagicMock(side_effect=None)
    handler.parse_assembly_file_for_labels()
    handler.open_file.assert_called_once()
    handler._find_all_labels.assert_called_once()
    handler.close_file.assert_called_once()


def test_close_on_unopened_file(handler):
    assert handler._file is None
    handler.close_file()
    assert handler._file is None


def test_close_on_open_file(handler):
    handler._file = MagicMock()
    mock_close = MagicMock(side_effect=None)
    handler._file.close = mock_close
    handler.close_file()
    mock_close.assert_called_once()
    assert handler._file is None
