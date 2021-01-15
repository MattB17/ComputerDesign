import pytest
from unittest.mock import patch, MagicMock, mock_open
from Assembler.Assembler import Assembler


PARSER_STR = "Assembler.Assembler.Parser"
HACK_FILE = "/path/to/assembly/file.hack"


@pytest.fixture(scope="function")
def parser():
    parser = MagicMock()
    parser.open_file = MagicMock(side_effect=None)
    parser.close_file = MagicMock(side_effect=None)
    return parser


@pytest.fixture(scope="function")
def assembler(parser):
    assembly_file = "/path/to/assembly/file.asm"
    with patch(PARSER_STR) as mock_parser:
        mock_parser.return_value = parser
        assembler = Assembler(assembly_file)
    mock_parser.assert_called_once_with(assembly_file)
    return assembler


def test_instantiation(parser, assembler):
    assert assembler._parser == parser
    assert assembler.get_machine_file_path() == HACK_FILE
    assert assembler._machine_file is None


def test_start_assembly(parser, assembler):
    with patch("builtins.open", mock_open()) as mock_file:
        assembler.start_assembly()
    parser.open_file.assert_called_once()
    mock_file.assert_called_once_with(HACK_FILE, 'w')


def test_finish_assembly_unopened_file(parser, assembler):
    assert assembler._machine_file is None
    assembler.finish_assembly()
    parser.close_file.assert_called_once()
    assert assembler._machine_file is None


def test_finish_assembly_open_file(parser, assembler):
    mock_file = MagicMock()
    assembler._machine_file = mock_file
    mock_file.close = MagicMock(side_effect=None)
    assert assembler._machine_file is not None
    assembler.finish_assembly()
    parser.close_file.assert_called_once()
    mock_file.close.assert_called_once()
    assert assembler._machine_file is None
