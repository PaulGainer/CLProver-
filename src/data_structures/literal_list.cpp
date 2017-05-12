/*==============================================================================
	File			: literal_list.cpp
	Author			: Paul Gainer
	Created			: 23/06/2014
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
#include <iostream>
#include <iterator>

#include "data_structures/literal_list.h"

#include "clprover/define.h"

/*==========================================================================
	Public Functions
==========================================================================*/
LiteralList* LiteralList::negation()
{
	std::list<Literal*>* new_list = new std::list<Literal*>(*list);
	std::list<Literal*>::iterator it = new_list->begin();
	std::list<Literal*>::iterator end = new_list->end();
	while(it != end)
	{
		Literal* this_literal = *it;
		this_literal = this_literal->complementary_literal;
	}
	LiteralList* new_literal_list = new LiteralList();
	new_literal_list->list = new_list;
	return new_literal_list;
}

Literal* LiteralList::getMaximalLiteral()
{
	return list == nullptr ? nullptr : list->back();
}

LiteralList* LiteralList::removeMaximalLiteral()
{
	LiteralList* new_literal_list = new LiteralList();

	if(list->size() <= 1)
	{
		// the list is empty so return the new empty onne
		return new_literal_list;
	}
	else
	{
		// the list is not empty so copy it
		std::list<Literal*>* new_list = new std::list<Literal*>(*list);
		new_list->pop_back();
		new_literal_list->list = new_list;
		return new_literal_list;
	}
}

void LiteralList::addLiteral(Literal* new_literal)
{
	if(list == nullptr)
	{
		list = new std::list<Literal*>;
	}
	// find the correct location for the literal
	std::list<Literal*>::iterator here =
		std::find_if(list->begin(), list->end(),
			[&](Literal* l)
			{
				return new_literal->rank < l->rank;
			});
	/* if this is the end of the list, or if this is not the end of the list and
	   this literal is not a duplicate, then add the literal to the list */
	if((here == list->end()) || (*here != new_literal))
	{
		list->insert(here, new_literal);
	}
	return;
}

bool LiteralList::isSubsetOf(LiteralList* that)
{
	// the empty set is a subset of any set
	if(list == nullptr)
	{
		return true;
	}
	// a non-empty set is not a subset of the empty set
	else if(that->list == nullptr)
	{
		return false;
	}
	/* this list has a greater maximal literal and as such cannot be a subset
	   of the given list */
	else if(list->back()->rank > that->list->back()->rank)
	{
		return false;
	}
	// return true if the given list contains this list
	return std::includes(that->list->begin(), that->list->end(), list->begin(),
		list->end(),
		[](Literal* this_literal, Literal* that_literal)
		{
			return this_literal->rank < that_literal->rank;
		});
}

bool LiteralList::isSubsetOfNegationOf(LiteralList* that)
{
	// the empty set is a subset of any set
	if(list == nullptr)
	{
		return true;
	}
	// a non-empty set is not a subset of the empty set
	else if(that->list == nullptr)
	{
		return false;
	}
	/* this list has a greater maximal literal and as such cannot be a subset
		 of the given list */
	else if(list->back()->rank > that->list->back()->rank)
	{
		return false;
	}
	// return true if the negation of the given list contains this list
	return std::includes(that->list->begin(), that->list->end(), list->begin(),
		list->end(),
		[](Literal* this_literal, Literal* that_literal)
		{
			return this_literal->rank < that_literal->complementary_literal->rank;
		});
}

/*==========================================================================
	Public Static Functions
==========================================================================*/
std::pair<LiteralList*, const bool>* LiteralList::unionLiterals(
	LiteralList* literal_list_1, LiteralList* literal_list_2)
{
	std::list<Literal*>* list_1 = literal_list_1->list;
	std::list<Literal*>* list_2 = literal_list_2->list;
	// list 1 is null so return list 2
	if(list_1 == nullptr)
	{
		delete literal_list_1;
		return new std::pair<LiteralList*, const bool>(literal_list_2,
			false);
	}
	// list 2 is null so return list 1
	else if(list_2 == nullptr)
	{
		delete literal_list_2;
		return new std::pair<LiteralList*, const bool>(literal_list_1,
			false);
	}
	std::list<Literal*>* new_list = new std::list<Literal*>();
	bool is_tautology = false;
	std::set_union(list_1->begin(), list_1->end(), list_2->begin(),
		list_2->end(), std::inserter(*new_list, new_list->begin()),
			[&](Literal* this_literal, Literal* that_literal)
			{
				if(this_literal->complementary_literal == that_literal)
				{
					is_tautology = true;
				}
				return this_literal->rank < that_literal->rank;
			});
	delete literal_list_1;
	delete literal_list_2;
	LiteralList* new_literal_list = new LiteralList();
	new_literal_list->list = new_list;
	return new std::pair<LiteralList*, const bool>(new_literal_list,
		is_tautology);
}

