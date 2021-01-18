import pytest
from unittest.mock import patch, MagicMock, mock_open, call
from Assembler.Assembler import Assembler


PARSER_STR = "Assembler.Assembler.Parser"
HANDLER_STR = "Assembler.Assembler.SymbolHandler"
HACK_FILE = "/path/to/assembly/file.hack"
INSTRUCTION_STR = "Assembler.Assembler.InstructionHandler"


@pytest.fixture(scope="function")
def parser():
    parser = MagicMock()
    parser.open_file = MagicMock(side_effect=None)
    parser.close_file = MagicMock(side_effect=None)
    return parser


@pytest.fixture(scope="function")
def symbol_handler():
    handler = MagicMock()
    handler.parse_assembly_file_for_labels = MagicMock(side_effect=None)
    return handler


@pytest.fixture(scope="function")
def assembler(parser, symbol_handler):
    assembly_file = "/path/to/assembly/file.asm"
    with patch(PARSER_STR) as mock_parser, \
        patch(HANDLER_STR) as mock_handler:
        mock_parser.return_value = parser
        mock_handler.return_value = symbol_handler
        assembler = Assembler(assembly_file)
    mock_parser.assert_called_once_with(assembly_file)
    mock_handler.assert_called_once_with(assembly_file)
    return assembler


def test_instantiation(parser, symbol_handler, assembler):
    assert assembler._parser == parser
    assert assembler._symbol_handler == symbol_handler
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


def test_execute_first_pass(symbol_handler, assembler):
    assembler.execute_first_pass()
    symbol_handler.parse_assembly_file_for_labels.assert_called_once()


@patch(INSTRUCTION_STR)
def test_assemble_next_instruction(handler, assembler):
    mock_handler = MagicMock()
    handler.return_value = mock_handler
    mock_instruction = MagicMock()
    mock_handler.get_instruction = MagicMock(return_value=mock_instruction)
    mock_instruction.get_binary_instruction = MagicMock(
        return_value="1111000010010111")
    mock_file = MagicMock()
    assembler._machine_file = mock_file
    mock_file.write = MagicMock(side_effect=None)
    instruction_str = "D=D+M;JMP // Some comment"
    assembler.assemble_next_instruction(instruction_str)
    handler.assert_called_once_with(instruction_str)
    mock_handler.get_instruction.assert_called_once()
    mock_instruction.get_binary_instruction.assert_called_once()
    mock_file.write.assert_called_once_with("1111000010010111\n")


def test_run_conversion_empty_file(parser, assembler):
    parser.get_next_assembly_line = MagicMock(return_value=None)
    assembler.assemble_next_instruction = MagicMock()
    assembler.run_conversion()
    parser.get_next_assembly_line.assert_called_once()
    assembler.assemble_next_instruction.assert_not_called()


def test_run_conversion_one_line_file(parser, assembler):
    line1 = "D=D+M // adding RAM[A] to D register"
    parser.get_next_assembly_line = MagicMock(side_effect=(line1, None))
    assembler.assemble_next_instruction = MagicMock(side_effect=None)
    assembler.run_conversion()
    assert parser.get_next_assembly_line.call_count == 2
    assembler.assemble_next_instruction.assert_called_once_with(line1)


def test_run_conversion_multi_line_file(parser, assembler):
    line1 = "D=D+M // adding RAM[A] to D register"
    line2 = "M=D"
    line3 = "0;JMP // unconditional jump"
    parser.get_next_assembly_line = MagicMock(
        side_effect=(line1, line2, line3, None))
    assembler.assemble_next_instruction = MagicMock(side_effect=None)
    assembler.run_conversion()
    assert parser.get_next_assembly_line.call_count == 4
    assemble_calls = [call(line1), call(line2), call(line3)]
    assembler.assemble_next_instruction.assert_has_calls(assemble_calls)
    assert assembler.assemble_next_instruction.call_count == 3

def test_assemble(assembler):
    assembler.start_assembly = MagicMock(side_effect=None)
    assembler.run_conversion = MagicMock(side_effect=None)
    assembler.finish_assembly = MagicMock(side_effect=None)
