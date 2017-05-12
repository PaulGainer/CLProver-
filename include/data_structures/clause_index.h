/*==============================================================================
	ClauseIndex

	A multi-layered tree structure with each layer corresponding to one feature
	of a clause. Clauses having the same features are stored in a list at the
	corresponding leaf node.

	File			: clause_index.h
	Author			: Paul Gainer
	Created			: 29/06/2014
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
#ifndef CLAUSE_INDEX_H_
#define CLAUSE_INDEX_H_

#include "enums/clause_type.h"

class Clause;
class IndexNode;

/*==============================================================================
	ClauseIndex
==============================================================================*/
class ClauseIndex
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		ClauseIndex

		Reads the tree depth then |depth| integer parameters, each defining the
		size of each consecutive level of the tree. The tree is then built.
	--------------------------------------------------------------------------*/
	ClauseIndex(const int, ...);

	/*--------------------------------------------------------------------------
		~ClauseIndex
	--------------------------------------------------------------------------*/
	~ClauseIndex();

	/*==========================================================================
		Accessors/Mutators
	==========================================================================*/
	inline int size() const {return num_clauses;}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		addToIndex

		Retrieves the features of the clause then adds the clause to the list of
		clauses sharing its features.
	--------------------------------------------------------------------------*/
	void addToIndex(Clause*);

	/*--------------------------------------------------------------------------
		removeFromIndex

		Retrieves the features of the clause and removes the clause from the list of
		clauses sharing to its features.
	--------------------------------------------------------------------------*/
	void removeFromIndex(Clause*);

	/*--------------------------------------------------------------------------
		displayIndex

		Displays all of the clauses stored in the index.
	--------------------------------------------------------------------------*/
	void displayIndex();

	/*--------------------------------------------------------------------------
		filterByMaxLiteral

		Returns all the clauses in the index having the literal with the given
		rank as their maximal literal.
	--------------------------------------------------------------------------*/
	std::list<IndexNode*>* filterByMaxLiteral(int, std::list<ClauseType>*);

	/*--------------------------------------------------------------------------
		filterByMaxLiteralAndLength

		Returns all the clauses in the index having a maximal literal with a
		rank no greater than the given rank and having a size no more than the
		given length.
	--------------------------------------------------------------------------*/
	std::list<IndexNode*>* filterByMaxLiteralAndLength(int, int,
		std::list<ClauseType>*);

	/*--------------------------------------------------------------------------
		filterByMinLiteralAndLength

		Returns all the clauses in the index having a maximal literal with a
		rank no less than the given rank and having a size no less than the
		given length.
	--------------------------------------------------------------------------*/
	std::list<IndexNode*>* filterByMinLiteralAndLength(int, int,
			std::list<ClauseType>*);

	/*--------------------------------------------------------------------------
		getNumClauses

		Accessor for num_clauses.
	--------------------------------------------------------------------------*/
	int getNumClauses() {return num_clauses;}

	/*--------------------------------------------------------------------------
		getNextSmallestClause

		Returns the smallest clause currently stored in the index.
	--------------------------------------------------------------------------*/
	Clause* getNextSmallestClause();

	/*--------------------------------------------------------------------------
		resolve

		Returns the list of all simplified non-tautological resolvents derivable
		between the given clause and compatible clauses in the index, using
		IRES1/GRES1/CRES1/CRES2/CRES3/CRES4/CRES5.
	--------------------------------------------------------------------------*/
	std::list<Clause*>* resolve(Clause*);

private:
	/*==========================================================================
		Private Structs
	==========================================================================*/
	/*--------------------------------------------------------------------------
		InternalNode

		An internal index tree node.
	--------------------------------------------------------------------------*/
	struct InternalNode;

	/*--------------------------------------------------------------------------
		InternalNodeTerminal

		An internal index tree node in the penultimate level of the index tree.
	---------------------------------------------------------------------------*/
	struct InternalNodeTerminal;

	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The depth of the index tree structure.
	--------------------------------------------------------------------------*/
	const int feature_count;

	/*--------------------------------------------------------------------------
		The number of clauses stored in the index.
	--------------------------------------------------------------------------*/
	int num_clauses = 0;

	/*--------------------------------------------------------------------------
		The maximum size of each tree level.
	--------------------------------------------------------------------------*/
	int* max_feature_values;

	/*--------------------------------------------------------------------------
		The length of the longest clause stored in the index.
	--------------------------------------------------------------------------*/
	int max_clause_length = 0;

	/*--------------------------------------------------------------------------
		The root of the index tree.
	--------------------------------------------------------------------------*/
	InternalNode** root;

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		buildNextLevel

		Builds the levels of the index tree. Recursively builds levels of
		internal nodes until the penultimate layer of internal nodes and its
		leaf nodes are built.
	--------------------------------------------------------------------------*/
	InternalNode** buildNextLevel(int, InternalNode*);

	/*--------------------------------------------------------------------------
		deleteNextLevel

		Removes the levels of the index tree. Recursively removes levels of
		internal nodes until the penultimate level of internal nodes and its
		leaf nodes are removed.
	--------------------------------------------------------------------------*/
	void deleteNextLevel(int, InternalNode**);

	/*--------------------------------------------------------------------------
		getClauseList

		Gets the list of clauses having the features defined by the given
		feature vector.
	--------------------------------------------------------------------------*/
	void getClauseList(int*, std::list<IndexNode*>*);

	/*--------------------------------------------------------------------------
		getClauseListNextFeature

		Recursively traverses the index tree and returns the list of clauses at
		the corresponding leaf node.
	--------------------------------------------------------------------------*/
	void getClauseListNextFeature(InternalNode**, int, int*,
		std::list<IndexNode*>*);

	/*--------------------------------------------------------------------------
		displayNextFeature

		Recursively traverses the tree, adding clauses stored at all leaf nodes
		to the list of clauses.
	--------------------------------------------------------------------------*/
	void displayNextFeature(InternalNode**, int, std::list<Clause*>*);

	/*==========================================================================
			Private Static Functions
	==========================================================================*/
	/*------------------------------------------------------------------------------
		displayTautology
	------------------------------------------------------------------------------*/
	static void displayTautology(Clause*, Clause*);
};

#endif
