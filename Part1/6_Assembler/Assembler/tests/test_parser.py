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
