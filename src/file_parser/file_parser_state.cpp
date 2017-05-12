/*==============================================================================
	File			: file_parser_state.cpp
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
#include <algorithm>

#include "file_parser/file_parser.h"
#include "file_parser/file_parser_state.h"
#include "file_parser/file_parser_transition.h"

/*==============================================================================
	Public Static Member Variables
==============================================================================*/
FileParserState FileParserState::TERMINAL_STATE = FileParserState();

/*==============================================================================
	Public Functions
==============================================================================*/
void FileParserState::addTransition(const FileParserTransition& new_transition)
{
	transitions.push_back(new_transition);
}

/*==============================================================================
	Private Functions
==============================================================================*/
FileParserState* FileParserState::makeTransition(const char input_character)
	const
{
	FileParserState* next_state = nullptr;
	auto it = std::find_if(transitions.begin(), transitions.end(),
		[&](FileParserTransition transition)
		{
			return transition.isValidInput(input_character);
		});
	if(it != transitions.end())
	{
		if((*it).transition_action != nullptr)
		{
			// execute the transition actions
			(*it).transition_action(input_character);
		}
		// move to the next state
		next_state = (*it).next_state;
		if(next_state->entry_action != nullptr)
		{
			// execute the state entry actions
			next_state->entry_action();
		}
	}
	return next_state;
}

std::string FileParserState::getValidInputString() const
{
	std::string valid_inputs;
	int last_transition = transitions.size() - 1;
	auto it = transitions.begin();
	for(int i = 0; i <= last_transition; i++)
	{
		FileParserTransition transition = *it;
		char lower = transition.input_range_lower;
		char upper = transition.input_range_upper;
		if(lower == upper)
		{
			valid_inputs += FileParser::asciiToString(lower);
		}
		else
		{
			valid_inputs.push_back('[');
			valid_inputs += FileParser::asciiToString(lower);
			valid_inputs.push_back('-');
			valid_inputs += FileParser::asciiToString(upper);
			valid_inputs.push_back(']');
		}
		if(i < last_transition)
		{
			valid_inputs += " | ";
		}
		it++;
	}
	return valid_inputs;
}

