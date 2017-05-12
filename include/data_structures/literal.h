/*==============================================================================
	Literal

	A data structure to represent an atom or its negation. Each	literal has an
	identifier, a boolean indicating its polarity, a pointer to its
	complementary literal, an integer rank and a boolean set to	true if the
	literal appears in a parsed clause.

	File			: literal.h
	Author			: Paul Gainer
	Created			: 16/06/2014
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
#ifndef LITERAL_H_
#define LITERAL_H_

#include <string>
#include <utility>

#include "enums/clause_type.h"

/*==============================================================================
	Literal
==============================================================================*/
struct Literal
{
	/*==========================================================================
		Struct Static Member Variables
	==========================================================================*/
	static int next_literal_rank;

	/*==========================================================================
		Struct Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Set to true if this literal appears in a parsed clause.
	--------------------------------------------------------------------------*/
	bool is_used;

	/*--------------------------------------------------------------------------
		The name of the literal.
	--------------------------------------------------------------------------*/
	std::string identifier;

	/*--------------------------------------------------------------------------
		Set to true if the literal is positive or false if negative.
	--------------------------------------------------------------------------*/
	bool polarity;

	/*--------------------------------------------------------------------------
		A pointer to the complementary literal of this literal.
	--------------------------------------------------------------------------*/
	Literal* complementary_literal;

	/*--------------------------------------------------------------------------
		Used to apply a literal ordering.
	--------------------------------------------------------------------------*/
	int rank;

	/*==========================================================================
		Struct Constructor/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Literal

		Creates a new literal with the given identifier, polarity and pointer
		to its complementary literal.
	--------------------------------------------------------------------------*/
	Literal(const std::string&, const bool, Literal*);

	/*==========================================================================
		Struct Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		displayLiteral

		Send a representation of this literal to standard output.
	--------------------------------------------------------------------------*/
	void displayLiteral();

	/*==========================================================================
		Struct Static Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		constructLiteralPair

		Constructs and returns a pair of literals with a given identifier. The
		first element of the pair is the positive literal and the second its
		complementary negative literal.
	--------------------------------------------------------------------------*/
	static std::pair<Literal*, Literal*>* constructLiteralPair(
		std::string);

	/*--------------------------------------------------------------------------
		constructLiteralPairWithOrdering

		Constructs and returns a pair of literals with a given identifier. The
		first element of the pair is the positive literal and the second its
		complementary negative literal.

		Used to construct literals if an ordering has been applied to the
		literals.
	--------------------------------------------------------------------------*/
	static std::pair<Literal*, Literal*>* constructLiteralPairWithOrdering(
		std::string );
};

#endif
