/*==============================================================================
	A collection of functions that operate on lists of clauses.

	File			: clause_list.h
	Author			: Paul Gainer
	Created			: 05/07/2014
	Last modified	: 17/04/2015
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
#ifndef CLAUSE_LIST_H_
#define CLAUSE_LIST_H_

#include <functional>
#include <list>
#include <set>

class Clause;
class ClauseIndex;

namespace clause_list
{
	/*--------------------------------------------------------------------------
		sortClauseList

		Sorts the given clause list by type (UNIVERSAL < INTIAL < POSITIVE <
		NEGATIVE), then disjunction size and finally conjunction size.
	--------------------------------------------------------------------------*/
	void sortClauseList(std::list<Clause*>*);

	/*--------------------------------------------------------------------------
		backwardSubsumption

		Removes from the given clause index all of the clauses that are subsumed
		by a clause in the given list.
	--------------------------------------------------------------------------*/
	void backwardSubsumption(std::list<Clause*>*, ClauseIndex& clause_index);

	/*--------------------------------------------------------------------------
		backwardSubsumptionGetNext

		Removes from the given clause index all of the clauses that are subsumed
		by a clause in the given list. Removes all subsumed clauses from the
		given sets.
	--------------------------------------------------------------------------*/
	void backwardSubsumptionGetNext(std::list<Clause*>*, ClauseIndex&,
		std::set<Clause*, std::function<bool (Clause*, Clause*)>>*,
		std::set<Clause*, std::function<bool (Clause*, Clause*)>>*,
		std::set<Clause*, std::function<bool (Clause*, Clause*)>>*);

	/*--------------------------------------------------------------------------
		subsumption

		Returns all the clauses in the given list that are not subsumed by
		clauses in the given clause index.
	--------------------------------------------------------------------------*/
	std::list<Clause*>* subsumption(std::list<Clause*>*, ClauseIndex&);

	/*--------------------------------------------------------------------------
		selfSubsumption

		Removes from the given list all of the clauses that are not subsumed by
		another clause in the list.
	--------------------------------------------------------------------------*/
	std::list<Clause*>* selfSubsumption(std::list<Clause*>*, ClauseIndex&);

	/*--------------------------------------------------------------------------
		initialSelfSubsumption

		Removes from the given list all of the clauses that are not subsumed by
		another clause in the list.
	--------------------------------------------------------------------------*/
	std::list<Clause*>* initialSelfSubsumption(std::list<Clause*>*,
		ClauseIndex&);
}

#endif
