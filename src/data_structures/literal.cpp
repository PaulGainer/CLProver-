/*==============================================================================
	File			: literal.cpp
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
#include <iostream>
#include <string>

#include "data_structures/literal.h"

#include "clprover/global.h"

/*==============================================================================
	Struct Static Member Variables
==============================================================================*/
int Literal::next_literal_rank = 1;

/*==============================================================================
	Constructors/Deconstructor
==============================================================================*/
Literal::Literal(const std::string& identifier, const bool polarity,
	Literal* complementary_literal): is_used(false), identifier(identifier),
		polarity(polarity), complementary_literal(complementary_literal)
{
	if(g_increment_literal_ranks)
	{
		rank = next_literal_rank++;
	}
	else
	{
		rank = next_literal_rank--;
	}
}

/*==============================================================================
	Struct Functions
==============================================================================*/

void Literal::displayLiteral()
{
	std::cout << "Literal ";
	if(polarity == false)
	{
		std::cout << "~";
	}
	std::cout << identifier << "     [rank: " << rank << ", is present: " <<
		(is_used ? "true" : "false") << "]\n";
}

/*==============================================================================
	Struct Static Functions
==============================================================================*/

std::pair<Literal*, Literal*>* Literal::constructLiteralPair(
	std::string identifier)
{
	Literal* positive = new Literal(identifier, true, nullptr);
	Literal* negative = new Literal(identifier, false, positive);
	positive->complementary_literal = negative;
	return new std::pair<Literal*, Literal*>(positive, negative);
}

std::pair<Literal*, Literal*>* Literal::constructLiteralPairWithOrdering(
	std::string identifier)
{
	Literal* negative = new Literal(identifier, false, nullptr);
	Literal* positive = new Literal(identifier, true, negative);
	negative->complementary_literal = positive;
	return new std::pair<Literal*, Literal*>(positive, negative);
}
