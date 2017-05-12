/*==============================================================================
	FileParser

	Facilitates the construction of a finite state machine to parse input files.
	A parser consists of an initial state, 0 or more other regular states, and
	a terminal state.

	Each state can have a set of entry actions, in the form of an std::function
	or lambda, and each transition can have a set of actions that are performed
	when transitioning to the next state.

	File			: file_parser.h
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
#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#include <fstream>
#include <string>

#include "file_parser/file_parser_state.h"
#include "file_parser/file_parser_transition.h"

/*==============================================================================
	FileParser
==============================================================================*/
class FileParser
{
public:
	/*==========================================================================
		Public Constants
	==========================================================================*/
	static const bool IGNORE_WHITESPACE = true;
	static const bool PARSE_WHITESPACE = false;
	static const bool VERBOSE = true;
	static const bool NON_VERBOSE = false;

	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		FileParser

		Constructs a new FileParser with the given executable name, a boolean
		set to true for verbose parsing and a boolean set to true to ignore
		whitespace when parsing.
	--------------------------------------------------------------------------*/
	FileParser(const std::string&, const bool, const bool);

	/*--------------------------------------------------------------------------
		~FileParser
	--------------------------------------------------------------------------*/
	~FileParser();

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		setInputFile

		Loads the input file with the given name. Returns true if the input file
		was found or false otherwise.

		If an input file has already been loaded then it is closed and the new
		file is opened.
	--------------------------------------------------------------------------*/
	bool setInputFile(const std::string&);

	/*--------------------------------------------------------------------------
		setInitialState

		Sets the state in which parsing will begin. This state must be set
		before parse is called.
	--------------------------------------------------------------------------*/
	inline void setInitialState(FileParserState& state) {initial_state = &state;}

	/*--------------------------------------------------------------------------
		parse

		If an input file and initial state have been set then begins the parsing
		of the file. Returns true if the file parsed successfully or false
		otherwise.
	--------------------------------------------------------------------------*/
	bool parse();

	/*--------------------------------------------------------------------------
		haltWithError

		Calling this function causes the next transition to fail. Parsing
		terminates unsuccessfully and the given error message is displayed.

		This can be used to terminate parsing from within a state or transition
		by calling this function within a function/lambda.
	--------------------------------------------------------------------------*/
	void haltWithError(const std::string&);

private:
	/*==========================================================================
		Private Constants
	==========================================================================*/
	static const std::string ASCII_CODES[33];

	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The state where parsing begins.
	--------------------------------------------------------------------------*/
	FileParserState* initial_state;

	/*--------------------------------------------------------------------------
		The input file to parse.
	--------------------------------------------------------------------------*/
	std::ifstream input_file;

	/*--------------------------------------------------------------------------
		The custom error message to be displayed when calling haltWithError.
	--------------------------------------------------------------------------*/
	std::string error_message;

	/*--------------------------------------------------------------------------
		The name of the input file.
	--------------------------------------------------------------------------*/
	std::string input_file_name;

	/*--------------------------------------------------------------------------
		The name of the executable.
	--------------------------------------------------------------------------*/
	const std::string exec_name;

	/*--------------------------------------------------------------------------
		Set to true to send extra parse information to standard output.
	--------------------------------------------------------------------------*/
	const bool verbose;

	/*--------------------------------------------------------------------------
		Set to true to ignore whitespace when parsing.
	--------------------------------------------------------------------------*/
	const bool ignore_whitespace;

	/*--------------------------------------------------------------------------
		Set to true if an input file has been successfully opened.
	--------------------------------------------------------------------------*/
	bool input_file_set;

	/*--------------------------------------------------------------------------
		Temporarily set to true to terminate the parsing process.
	--------------------------------------------------------------------------*/
	bool halt;

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		closeInputFile

		If any input file has been opened then it will be closed.
	--------------------------------------------------------------------------*/
	void closeInputFile();

	/*--------------------------------------------------------------------------
		displayError

		Sends the given error message to standard output.
	--------------------------------------------------------------------------*/
	void displayError(const std::string&) const;

	/*--------------------------------------------------------------------------
		displayParseLocation

		Sends to standard output the contents of the file that have been parsed
		up to the given position in the file.

		Used to show the location of parsing errors in the input.
	--------------------------------------------------------------------------*/
	void displayParseLocation(int);

	/*==========================================================================
		Private Static Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		asciiToString

		If the given char value corresponds to an ASCII character that can be
		displayed then this is returned as a string, otherwise a string
		representation of the type of character is returned e.g. TAB for
		(char)9.
	--------------------------------------------------------------------------*/
	static std::string asciiToString(const char);

	static const std::string integerToString(const int n);
	
	friend class FileParserState;
};

#endif
