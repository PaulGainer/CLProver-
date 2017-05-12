/*==============================================================================
	Clause

	A data structure to represent a DSNF_CL^MV clause, comprising the left side
	conjunction of literals, right side disjunction of literals, list of agents,
	clause type, justification and coalition vector.

	File			: clause.h
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
#ifndef CLAUSE_H_
#define CLAUSE_H_

#include <list>
#include <vector>
#include <string>

#include "data_structures/literal_list.h"

#include "clprover/define.h"

#include "enums/clause_type.h"

struct Justification;
class ClauseIndex;
class IndexNode;
class Literal;

/*==============================================================================
	Clause
==============================================================================*/
class Clause
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Clause

		Constructs and returns a new clause. If the right side conjunction is
		empty and this is a coalition clause then archive this clause and
		return the rewritten universal clause instead.
	--------------------------------------------------------------------------*/
	Clause(LiteralList*, std::list<int>*, LiteralList*, ClauseType,
		Justification*, std::vector<int>*);

	/*--------------------------------------------------------------------------
		~Clause
	--------------------------------------------------------------------------*/
	~Clause();

	/*==========================================================================
		Operator Overloads
	==========================================================================*/
	/*--------------------------------------------------------------------------
		operator <

		Allows the sorting of clauses by clause type (U<I<P<N), then disjunction
		size, then conjunction size and finally by the number of agents.
	--------------------------------------------------------------------------*/
	bool operator<(const Clause&);

	/*==========================================================================
		Accessors/Mutators
	==========================================================================*/
	inline int getIdentifier() const {return identifier;}
	inline LiteralList* getLeft() const {return left;}
	inline std::list<int>* getAgents() const {return agents;}
	inline LiteralList* getRight() const {return right;}
	inline ClauseType getClauseType() const	{return clause_type;}
	inline Justification* getJustification() const {return justification;}
	/*--------------------------------------------------------------------------
		size

		Returns the size of a clause, where the size of a clause is calculated
		as conjunction size plus disjunction size.
	--------------------------------------------------------------------------*/
	inline int size() const{return left->size() + right->size();}
	/*--------------------------------------------------------------------------
		isActive/setActive/setInactive

		Used during unit propagation.
	--------------------------------------------------------------------------*/
	inline bool isActive() const {return active;}
	inline void setActive() {active = true;}
	inline void setInactive() {active = false;}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		addMoveToVector

		Adds a move to the coalition vector and creates a new vector if this
		is the first move added.
	--------------------------------------------------------------------------*/
	inline void addMoveToVector(const int i)
	{
		if(coalition_vector == nullptr)
		{
			coalition_vector = new std::vector<int>();
		}
		coalition_vector->push_back(i);
	}

	/*--------------------------------------------------------------------------
		addLiteral

		Adds a new literal to the corresponding literal list.
	--------------------------------------------------------------------------*/
	inline void addLiteral(Literal* new_literal, const bool is_left)
	{
		if(is_left)
		{
			left->addLiteral(new_literal);
		}
		else
		{
			right->addLiteral(new_literal);
		}
	}

	/*--------------------------------------------------------------------------
		calculateFeatures

		Returns a feature vector characterizing the clause.
	--------------------------------------------------------------------------*/
	int* calculateFeatures();

	/*--------------------------------------------------------------------------
		displayClause

		Send a representation of this clause to standard output.
	--------------------------------------------------------------------------*/
	void displayClause();

	/*--------------------------------------------------------------------------
		subsumes

		Returns true if this clause subsumes the given clause. Subsumption
		between coalition clauses is determined using coalition vectors.

		If C_L is the conjunction of a clause C, C_R is the disjunction and C_V
		is the coalition vector, then a clause C subsumes a clause C' if:

		C_R \subseteq C'_R if both are initial clauses.
		C_R \subseteq C'_R if C is universal and C is initial.
		C_R \subseteq C'_R or C_R \subseteq \negC'_L if C is universal and C' is
			a coalition clause.
		C_R \subseteq C'_R and C_L \subseteq C'_L and C_V subsumes C_V' and
			C and C' are both coalition clauses of the same type.
	--------------------------------------------------------------------------*/
	bool subsumes(Clause*);

	/*--------------------------------------------------------------------------
		initialSubsumes

		Returns true if this clause subsumes the given clause. Subsumption
		between coalition clauses is determined using agent lists.

		If C_L is the conjunction of a clause C, C_R is the disjunction and C_M
		is the list of agents, then a clause C subsumes a clause C' if:

		C_R \subseteq C'_R if both are initial clauses.
		C_R \subseteq C'_R if C is universal and C is initial.
		C_R \subseteq C'_R or C_R \subseteq \negC'_L if C is universal and C' is
			a coalition clause.
		C_R \subseteq C'_R and C_L \subseteq C'_L and C_M \subseteq C_M' and
			C and C' are both positive clauses.
		C_R \subseteq C'_R and C_L \subseteq C'_L and C_M' \subseteq C_M and
			C and C' are both negative clauses.
	--------------------------------------------------------------------------*/
	bool initialSubsumes(Clause*);

	/*--------------------------------------------------------------------------
		relativeComplementAgents

		Returns the relative complement of this list of agents and the list of
		agents in the given clause.
	--------------------------------------------------------------------------*/
	std::list<int>* relativeComplementAgents(Clause*);

	/*--------------------------------------------------------------------------
		unionAgents

		Returns the union of this list of agents and the list of agents in the
		given clause.
	--------------------------------------------------------------------------*/
	std::list<int>* unionAgents(Clause*);

	/*--------------------------------------------------------------------------
		intersectionAgents

		Returns the intersection of this list of agents and the list of agents
		in the given clause.
	--------------------------------------------------------------------------*/
	std::list<int>* intersectionAgents(Clause*);

	/*--------------------------------------------------------------------------
		mergeCoalitions

		Returns a pair, where the first is the merger of this coalition vector
		and the coalition vector of the given clause, and the second is a
		boolean which is true if the merge was permitted.

		For a coalition vectors V and V' a merge is permitted if for each
		non-zero move in V there is an equal corresponding move in V', with the
		additional condition that all negative moves in the resulting vector
		must be equal.
	--------------------------------------------------------------------------*/
	std::pair<std::vector<int>*, const bool>* mergeCoalitions(Clause*);

	/*--------------------------------------------------------------------------
		nullifyLeft

		Used during unit propagation to nullify a list of literals if it is
		empty.
	--------------------------------------------------------------------------*/
	void nullifyLeft();

	/*==========================================================================
		Public Static Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getNextIdentifier

		Returns the next unused clause identifier.
	--------------------------------------------------------------------------*/
	inline static int getNextIdentifier() {return next_identifier;}

private:
	/*==========================================================================
		Private Constants
	==========================================================================*/
	/*--------------------------------------------------------------------------
		String representations of the clause types.
	--------------------------------------------------------------------------*/
	static const std::string CLAUSE_TYPE_STRINGS[4];

	/*--------------------------------------------------------------------------
		String representations of the inference rules.
	--------------------------------------------------------------------------*/
	static const std::string INFERENCE_RULE_STRINGS[9];

	/*==========================================================================
		Private Static Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The next unused clause identifier.
	--------------------------------------------------------------------------*/
	static int next_identifier;

	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The unique clause identifier.
	--------------------------------------------------------------------------*/
	int identifier;

	/*--------------------------------------------------------------------------
		The coalition vector.
	--------------------------------------------------------------------------*/
	std::vector<int>* coalition_vector;

	/*--------------------------------------------------------------------------
		The left side conjunction.
	--------------------------------------------------------------------------*/
	LiteralList* left;

	/*--------------------------------------------------------------------------
		The list of agents.
	--------------------------------------------------------------------------*/
	std::list<int>* agents;

	/*--------------------------------------------------------------------------
		The right side disjunction.
	--------------------------------------------------------------------------*/
	LiteralList* right;

	/*--------------------------------------------------------------------------
		The type of this clause.
	--------------------------------------------------------------------------*/
	ClauseType clause_type;

	/*--------------------------------------------------------------------------
		The clause justification. Set to null if this was a given clause.
	--------------------------------------------------------------------------*/
	Justification* justification;

	/*--------------------------------------------------------------------------
		Used during unit propagation.
	--------------------------------------------------------------------------*/
	bool active;

	/*--------------------------------------------------------------------------
		A pointer to the index node at which the clause is stored. St to null if
		the clause is not stored in an index.
	--------------------------------------------------------------------------*/
	IndexNode* index_node;

	/*==========================================================================
		Private Constructors
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Used to create a new clause without rewriting.
	--------------------------------------------------------------------------*/
	Clause() {}

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		coalitionSubsumes

		Returns true if this coalition vector subsumes the coalition vector of
		the given clause.

		A coalition vector V subsumes a coalition vector V' if for each non-zero
		move in V there is an equal corresponding move in V'.
	--------------------------------------------------------------------------*/
	bool coalitionSubsumes(Clause*);

	/*--------------------------------------------------------------------------
		isAgentSubsetOf

		Returns true if this list of agents is a subset of the list of agents in
		the given clause.
	--------------------------------------------------------------------------*/
	bool isAgentSubsetOf(Clause*);

	/*==========================================================================
		Friend Classes
	==========================================================================*/
	friend class ClauseIndex;
};

#endif
