/*==============================================================================
	IndexNode

	A node in doubly linked list, and the leaf node of a ClauseIndex. Each
	IndexNode has a pointer to the next and previous nodes in the list, which
	are set to null if the respectively the last or first node in the list. A
	Clause is stored at each node.

	File			: index_node.h
	Author			: Paul Gainer
	Created			: 30/06/2014
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
#ifndef INDEX_NODE_H_
#define INDEX_NODE_H_

#include "data_structures/clause.h"

class Clause;

/*==============================================================================
	FileParserState
==============================================================================*/
struct IndexNode
{
	/*==========================================================================
		Struct Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		A pointer to the clause stored at this node.
	--------------------------------------------------------------------------*/
	Clause* clause;
	/*--------------------------------------------------------------------------
		A pointer to the next node in the list.
	--------------------------------------------------------------------------*/
	IndexNode* next;
	/*--------------------------------------------------------------------------
		A pointer to the previous node in the list.
	--------------------------------------------------------------------------*/
	IndexNode* prev;

	/*==========================================================================
		Struct Constructor
	==========================================================================*/
	IndexNode(Clause* clause, IndexNode* next, IndexNode* prev):
		clause(clause), next(next), prev(prev) {}
};

#endif
