/*==============================================================================
	Functions to parse the input and literal ordering files.

	File			: parsing.h
	Author			: Paul Gainer
	Created			: 22/06/2014
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
#ifndef PARSING_H_
#define PARSING_H_

#include <string>
#include <list>
#include <set>

#include "data_structures/clause.h"

/*==============================================================================
	Function Prototypes
==============================================================================*/
/*------------------------------------------------------------------------------
	parseOrderingFile

	Parses a file describing an ordering on literals. All parsed literals in
	this file will have a greater rank than any literals appearing only in
	parsed clauses.
------------------------------------------------------------------------------*/
bool parseOrderingFile(const std::string&);

/*------------------------------------------------------------------------------
	parseInputFile

	Parses an input file containing a list of DNCF_RES^> clauses and returns the
	list of parsed clauses.
------------------------------------------------------------------------------*/
std::list<Clause*>* parseInputFile(const std::string&);

/*------------------------------------------------------------------------------
	constructLiterals

	Construct a pair of positive/negative literals. Used to construct literals
	defined in a literal ordering file.
------------------------------------------------------------------------------*/
void constructLiterals(const std::string&);

/*------------------------------------------------------------------------------
	constructAndReturnLiteral

	If literals with the given identifier have not already been defined then
	a pair of positive and negative literals are created. The literal with the
	given polarity is returned.
------------------------------------------------------------------------------*/
Literal* constructAndReturnLiteral(const std::string& literal_name,
	const bool polarity);

/*------------------------------------------------------------------------------
	checkLiteralUse

	Identifies literals that appear in parsed clauses.
------------------------------------------------------------------------------*/
void checkLiteralUse(Clause*);

#endif
