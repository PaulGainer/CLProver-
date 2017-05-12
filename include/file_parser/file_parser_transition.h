/*==============================================================================
	FileParserTransition

	A state transition for a FileParser. Each transition has either a single
	character or a bounded range for valid character input, a function to
	execute when transitioning to the next state, and the state to transition to
	with valid input.

	File			: file_parser_transition.h
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
#ifndef FILE_PARSER_TRANSITION_H_
#define FILE_PARSER_TRANSITION_H_

#include <functional>

#include "file_parser/file_parser_state.h"

/*==============================================================================
	FileParserState
==============================================================================*/
class FileParserTransition
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Transition

		Creates a new state transition with a range of valid inputs (e.g. A-Z),
		a function to execute upon transition and a pointer to the next state.
	--------------------------------------------------------------------------*/
	FileParserTransition(const char lower, const char upper,
		const std::function<void(char)>& action, FileParserState& next):
		input_range_lower(lower), input_range_upper(upper),
		transition_action(action), next_state(&next) {}

	/*--------------------------------------------------------------------------
		Transition

		Creates a new state transition with a valid input, a function to execute
		upon transition and a pointer to the next state.
	--------------------------------------------------------------------------*/
	FileParserTransition(const char c, const std::function<void(char)>& action,
		FileParserState& next): input_range_lower(c),
		input_range_upper(c), transition_action(action), next_state(&next) {}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		isValidInput

		Returns true if the input character is valid input for this transition.
	--------------------------------------------------------------------------*/
	inline bool isValidInput(const char input_character) const
	{
		return input_character >= input_range_lower &&
			input_character <= input_range_upper;
	}

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The inclusive lower bound of the input range.
	--------------------------------------------------------------------------*/
	const char input_range_lower;

	/*--------------------------------------------------------------------------
		The inclusive upper bound of the input range.
	--------------------------------------------------------------------------*/
	const char input_range_upper;

	/*--------------------------------------------------------------------------
		The function to execute upon transition.
	--------------------------------------------------------------------------*/
	const std::function<void(char)> transition_action;

	/*--------------------------------------------------------------------------
		The state to move to upon transition.
	--------------------------------------------------------------------------*/
	FileParserState* next_state;

	friend class FileParser;
	friend class FileParserState;
};

#endif
