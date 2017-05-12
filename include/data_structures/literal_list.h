/*==============================================================================
	LiteralList

	An ordered list of literals in which the maximal literal is the last literal
	in the list.

	File			: literal_list.h
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
#ifndef LITERAL_LIST_H_
#define LITERAL_LIST_H_

#include <list>

#include "data_structures/literal.h"

/*==============================================================================
	LiteralList
==============================================================================*/
class LiteralList
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		LiteralList

		Creates a new empty literal list.
	--------------------------------------------------------------------------*/
	LiteralList(): list(nullptr) {}

	/*--------------------------------------------------------------------------
		LiteralList

		Copy constructor.
	--------------------------------------------------------------------------*/
	LiteralList(const LiteralList& other)
	{
		list = other.list ==
			nullptr ? nullptr : new std::list<Literal*>(*(other.list));
	}

	/*--------------------------------------------------------------------------
		~LiteralList
	--------------------------------------------------------------------------*/
	~LiteralList()
	{
		delete list;
	}

	/*==========================================================================
		Accessors/Mutators
	==========================================================================*/
	inline std::list<Literal*>* getList() const {return list;}
	inline int size() const {return (list == nullptr ? 0 : list->size());}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		negation

		For a list of literals L this function returns the list of literals L'
		where for each literal p in L there is \neg p in L'.
	--------------------------------------------------------------------------*/
	LiteralList* negation();

	/*--------------------------------------------------------------------------
		getMaximalLiteral

		Returns the maximal literal in the list.
	--------------------------------------------------------------------------*/
	Literal* getMaximalLiteral();

	/*--------------------------------------------------------------------------
		removeMaximalLiteral

		Returns a copy of the list with the maximal literal removed.
	--------------------------------------------------------------------------*/
	LiteralList* removeMaximalLiteral();

	/*--------------------------------------------------------------------------
		addLiteral

		Adds a literal to its corresponding place in the list. Duplicates are
		discarded.
	--------------------------------------------------------------------------*/
	void addLiteral(Literal*);

	/*--------------------------------------------------------------------------
		isSubsetOf

		Returns true if this list is a subset of the given list.
	--------------------------------------------------------------------------*/
	bool isSubsetOf(LiteralList*);

	/*--------------------------------------------------------------------------
		isSubsetOfNegationOf

		Returns true if this list is a subset of the negation of the given list.
	--------------------------------------------------------------------------*/
	bool isSubsetOfNegationOf(LiteralList*);

	/*==========================================================================
		Public Static Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		unionLiterals

		Returns a pair where the first element is the union of the two lists of
		literals, and the second is true if the union is a tautology.
	--------------------------------------------------------------------------*/
	static std::pair<LiteralList*, const bool>* unionLiterals(
			LiteralList*, LiteralList*);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The list of literals.
	--------------------------------------------------------------------------*/
	std::list<Literal*>* list;

	friend class Clause;
};

#endif
