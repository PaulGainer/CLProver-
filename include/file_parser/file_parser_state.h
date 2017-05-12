/*==============================================================================
	FileParserState

	A parse state for a FileParser. Each state has an identifier, an (optional)
	function to execute upon entering the state, and a list of transitions.

	File			: file_parser_state.h
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
#ifndef FILE_PARSER_STATE_H_
#define FILE_PARSER_STATE_H_

#include <functional>
#include <list>
#include <string>

class FileParserTransition;

/*==============================================================================
	FileParserState
==============================================================================*/
class FileParserState
{
public:
	/*==========================================================================
		Public Static Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The terminal state. Entry to this state concludes the parsing of a file.
	--------------------------------------------------------------------------*/
	static FileParserState TERMINAL_STATE;

	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		FileParserState

		Creates a new file parser state with the given identifier and entry
		action function.
	--------------------------------------------------------------------------*/
	FileParserState(const std::string& id,
		const std::function<void(void)>& action): identifier(id),
		entry_action(action), is_terminal(false) {}

	/*--------------------------------------------------------------------------
		FileParserState

		Creates a new file parser state with the given identifier an no entry
		action function.
	--------------------------------------------------------------------------*/
	FileParserState(const std::string& id): identifier(id),
		entry_action(nullptr), is_terminal(false) {}

	/*--------------------------------------------------------------------------
		~FileParserState
	--------------------------------------------------------------------------*/
	~FileParserState() {}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		addTransition

		Adds a new FileParserTransition to the list of transitions for this
		state.
	--------------------------------------------------------------------------*/
	void addTransition(const FileParserTransition&);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The list of transitions possible from this state.
	--------------------------------------------------------------------------*/
	std::list<FileParserTransition> transitions;

	/*--------------------------------------------------------------------------
		The name of the state displayed when output is verbose.
	--------------------------------------------------------------------------*/
	const std::string identifier;

	/*--------------------------------------------------------------------------
		The function to execute upon entry to the state. Set to null if there
		is no function to execute.
	--------------------------------------------------------------------------*/
	const std::function<void(void)> entry_action;

	/*--------------------------------------------------------------------------
		Set to true in the terminal state or false otherwise.
	--------------------------------------------------------------------------*/
	const bool is_terminal;

	/*==========================================================================
		Private Constructors
	==========================================================================*/
	/*--------------------------------------------------------------------------
		FileParserState

		Constructs the terminal state.
	--------------------------------------------------------------------------*/
	FileParserState(): is_terminal(true) {}

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		makeTransition

		If the given input character corresponds to a valid transition then the
		state to transition to is returned, or null otherwise.
	--------------------------------------------------------------------------*/
	FileParserState* makeTransition(const char) const;

	/*--------------------------------------------------------------------------
		getValidInputString

		Returns a string representation of all valid inputs for this state.
	--------------------------------------------------------------------------*/
	std::string getValidInputString() const;

	friend class FileParser;
	friend class FileParserTransition;
};

#endif
