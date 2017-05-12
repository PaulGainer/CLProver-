/*==============================================================================
	File			: file_parser.cpp
	Author			: Paul Gainer
	Created			: 10/09/2014
	Last modified	: 16/09/2014
==============================================================================*/
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/==============================================================================\
|   Copyright (C) 2014 Paul Gainer, University of Liverpool                    |
|                                                                              |
|   This file is part of CLProver++.                                           |
|                                                                              |
|   CLProver++ is free software; you can redistribute it and/or modify it      |
|   under the terms of the GNU General Public License as published by the Free |
|   Software Foundation, either version 3 of the License; or (at your option)  |
|   any later version.                                                         |
|                                                                              |
|   CLProver++ is distributed in the hope that it will be useful, but WITHOUT  |
|   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      |
|   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   |
|   more details.                                                              |
|                                                                              |
|   You should have received a copy of the GNU General Public License along    |
|   with CLProver++. If not, see <http:/www.gnu.org/licenses/>.                |
|                                                                              |
\==============================================================================/
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#include <iostream>
#include <list>
#include <sstream>

#include "file_parser/file_parser.h"

/*==============================================================================
	Private Constants
=============================================================================*/
const std::string FileParser::ASCII_CODES[33] =
{
	"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "TAB",
	"LF", "VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4",
	"NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US",
	"Space"
};

/*==============================================================================
		Public Constructors/Deconstructor
==============================================================================*/
FileParser::FileParser(const std::string& name, const bool v, const bool i):
	initial_state(nullptr), exec_name(name), verbose(v), ignore_whitespace(i),
	input_file_set(false), halt(false) {}

FileParser::~FileParser()
{
	// don't leave the input file open
	if(input_file_set)
	{
		input_file.close();
	}
}

/*==============================================================================
		Public Functions
==============================================================================*/
bool FileParser::setInputFile(const std::string& file_name)
{
	if(input_file_set)
	{
		// close the input file that is open
		if(verbose)
		{
			std::cout << "switching input file\n";
		}
		closeInputFile();
	}
	input_file.open(file_name.c_str());
	if(!input_file.good())
	{
		// cant find the file
		displayError("could not locate file '" + file_name + "'");
		input_file_set = false;
		return false;
	}
	if(verbose)
	{
		std::cout << "opening file '" << file_name << "'\n";
	}
	input_file_set = true;
	input_file_name = file_name;
	return true;
}

bool FileParser::parse()
{
	if(!input_file_set)
	{
		displayError("cannot parse: no input file has been set");
		return false;
	}
	else if(initial_state == nullptr)
	{
		displayError("cannot parse: no initial state has been set");
		return false;
	}
	else if(verbose)
	{
		std::cout << "parsing '" << input_file_name << "'\n";
	}

	input_file.clear();
	input_file.seekg(0, std::ios::beg);
	char input_character;
	int fpos = 0;
	bool done = false;
	bool parse_error = false;
	FileParserState* current_state = initial_state;
	FileParserState* last_state;
	if(current_state->is_terminal)
	{
		// began in a terminal state
		if(verbose)
		{
			std::cout << "initial state was terminal\n";
			closeInputFile();
			return true;
		}
	}
	if(current_state->entry_action != nullptr)
	{
		// actions for entry into initial state
		current_state->entry_action();
	}

	while(!done)
	{
		last_state = current_state;
		if(ignore_whitespace)
		{
			// ignore whitespace
			do
			{
				fpos++;
				input_character = input_file.get();
			} while(isspace(input_character));
		}
		else
		{
			fpos++;
			input_character = input_file.get();
		}
		// transition to the next state
		current_state = current_state->makeTransition(input_character);
		if(halt)
		{
			// halt has been called, display the error and terminate parsing
			if(verbose)
			{
				std::cout << "parsing halted: " <<
					(error_message.empty() ? "UNKNOWN ERROR" : error_message) <<
					std::endl;
				// display the last parse location
				displayParseLocation(fpos);
			}
			parse_error = true;
			done = true;
			halt = false;
			error_message = "";
		}
		else if(current_state == nullptr)
		{
			// there was no transition for the given input
			if(verbose)
			{
				displayError("unexpected input: expected " +
					(*last_state).getValidInputString());
				displayParseLocation(fpos);
			}
			parse_error = true;
			done = true;
		}
		else
		{
			if(current_state->is_terminal)
			{
				// this is a terminal state
				if(verbose)
				{
					std::cout << "moved from " << (*last_state).identifier <<
						" to terminal state with input " <<
						asciiToString(input_character) << std::endl;
				}
				done = true;
			}
			else
			{
				// this is not a terminal state
				if(verbose)
				{
					std::cout << "moved from " << (*last_state).identifier <<
						" to " << (*current_state).identifier <<
						" with input " << asciiToString(input_character) <<
						std::endl;
				}
			}
		}
	}
	closeInputFile();
	return !parse_error;
}

void FileParser::haltWithError(const std::string& error)
{
	error_message = error;
	halt = true;
}

/*==============================================================================
		Public Functions
==============================================================================*/
void FileParser::closeInputFile()
{
	if(verbose)
	{
		std::cout << "closing file '" << input_file_name << "'\n";
	}
	input_file_set = false;
	input_file.close();
}

void FileParser::displayParseLocation(int fpos)
{
	std::cout << "at offset " << fpos << " from beginning of file '" <<
		input_file_name << "'\n";
	std::cout << "==============================================" <<
		"==================================\n";
	input_file.seekg(0, std::ios::beg);
	while(fpos-- > 0)
	{
		char c = (char)input_file.get();
		if(c != EOF)
		{
			std::cout << c;
		}
		else
		{
			std::cout << "EOF";
		}
	}
	std::cout << "[<--UNEXPECTED INPUT]\n";
	std::cout << "==============================================" <<
		"==================================\n";
}

std::string FileParser::asciiToString(const char c)
{
	std::string ascii_string;
	if(c == EOF)
	{
		ascii_string = "EOF";
	}
	else if(c <= 32)
	{
		ascii_string = ASCII_CODES[(int)c] + " (";
		ascii_string += integerToString((int)c);
		ascii_string += ")";
	}
	else if(c == 127)
	{
		ascii_string = "DEL (127)";
	}
	else
	{
		ascii_string.push_back(c);
	}
	return ascii_string;
}

void FileParser::displayError(const std::string& message) const
{
	std::cout << exec_name << " error: " << message << std::endl;
}

const std::string FileParser::integerToString(const int n)
{
	std::stringstream stream;
	stream << n;
	return stream.str();
}
