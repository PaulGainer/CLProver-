/*==============================================================================
	File			: clause.cpp
	Author			: Paul Gainer
	Created			: 16/06/2014
	Last modified	: 26/02/2015
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
#include <string>

#include "clprover/global.h"

#include "data_structures/clause.h"
#include "data_structures/clause_index.h"
#include "data_structures/index_node.h"
#include "data_structures/justification.h"

/*==============================================================================
	Private Static Member Variables
==============================================================================*/
int Clause::next_identifier = 1;

const std::string Clause::CLAUSE_TYPE_STRINGS[4] = {"U", "I", "P", "N"};

const std::string Clause::INFERENCE_RULE_STRINGS[9] = {"IRES1", "GRES1", "CRES1",
	"CRES2", "CRES3", "CRES4", "CRES5", "RW1", "RW2"};

/*==============================================================================
	Public Constructors/Deconstructor
==============================================================================*/
Clause::Clause(LiteralList* left, std::list<int>* agents, LiteralList* right,
		ClauseType clause_type, Justification* justification,
		std::vector<int>* coalition_vector): active(true), index_node(nullptr)
{
	if(right->size() == 0 && (clause_type == ClauseType::POSITIVE ||
		clause_type == ClauseType::NEGATIVE))
	{
		/* this is a coalition clause with an empty disjunction so we will
		   rewrite it as a universal clause */
		g_rewrite_count++;
		Clause* c = new Clause();
		c->left = left;
		c->agents = agents;
		c->right = right;
		c->clause_type = clause_type;
		c->justification = justification;
		c->coalition_vector = coalition_vector;
		c->identifier = next_identifier++;
		Justification* j = new Justification(c->identifier, -1, nullptr,
			clause_type == ClauseType::POSITIVE ?
				InferenceRule::RW1 : InferenceRule::RW2);
		// archive the clause to be rewritten
		g_archive->push_back(c);
		LiteralList* r = new LiteralList();
		if(left->list != nullptr)
		{
			/* add the negation of literals in the conjunction to the
			   disjunction */
			std::for_each(left->list->begin(), left->list->end(),
				[&](Literal* l)
				{
					r->addLiteral(l->complementary_literal);
				});
		}
		left = new LiteralList();
		agents = new std::list<int>();
		coalition_vector = nullptr;
		right = r;
		clause_type = ClauseType::UNIVERSAL;
		justification = j;
	}
	identifier = next_identifier++;
	this->left = left;
	this->agents = agents;
	this->right = right;
	this->clause_type = clause_type;
	this->justification = justification;
	this->coalition_vector = coalition_vector;
}

Clause::~Clause()
{
	delete left;
	delete right;
	delete agents;
	delete coalition_vector;
	delete justification;
}
/*==============================================================================
	Operator Overloads
==============================================================================*/
bool Clause::operator<(const Clause& that)
{
	// sort by type first...
	if(clause_type == that.clause_type)
	{
		// ...then by disjunction size...
		if(right->size() == that.right->size())
		{
			// ...then by conjunction size...
			if(left->size() == that.left->size())
			{
				// and finally by the number of agents.
				int num_agents_this = (agents == nullptr) ? 0 : agents->size();
				int num_agents_that =
					(that.agents == nullptr) ? 0 : that.agents->size();
				return num_agents_this < num_agents_that;
			}
			else
			{
				return left->size() < that.left->size();
			}
		}
		else
		{
			return right->size() < that.right->size();
		}
	}
	else
	{
		return clause_type < that.clause_type;
	}
}

/*==============================================================================
	Public Functions
==============================================================================*/
int* Clause::calculateFeatures()
{
	int* feature_vector = new int[NUM_FEATURES];
	Literal* max_literal = right->getMaximalLiteral();
	feature_vector[0] = clause_type;
	feature_vector[1] = max_literal == nullptr ? 0 : max_literal->rank;
	feature_vector[2] = right->size() + left->size();
	return feature_vector;
}

void Clause::displayClause()
{
	std::list<Literal*>::iterator it, end;
	std::cout << "Clause " << identifier << " (" <<
		CLAUSE_TYPE_STRINGS[clause_type] << ")  ";
	if(left->size() > 0)
	{
		std::list<Literal*>* left_list = left->list;
		it = left_list->begin();
		end = left_list->end();

		std::cout << "(";
		while(it != end)
		{
			if(!(*it)->polarity)
			{
				std::cout << "~";
			}
			std::cout << (*it)->identifier;
			it++;
			if(it != end)
			{
				std::cout << " & ";
			}
		}
		std::cout << ")";
	}
	else if(clause_type == ClauseType::POSITIVE ||
			clause_type == ClauseType::NEGATIVE)
	{
		std::cout << "true";
	}

	if(clause_type == ClauseType::POSITIVE ||
		clause_type == ClauseType::NEGATIVE)
	{
		std::cout << "-->";

		if(g_display_modality_as_vector)
		{
			if(clause_type == ClauseType::POSITIVE ||
				clause_type == ClauseType::NEGATIVE)
			{
				std::cout << "[";
				if(coalition_vector != nullptr)
				{
					auto coalition_vector_it = coalition_vector->begin();
					auto coalition_vector_end = coalition_vector->end();
					while(coalition_vector_it != coalition_vector_end)
					{
						std::cout << *coalition_vector_it;
						coalition_vector_it++;
						if(coalition_vector_it != coalition_vector_end)
						{
							std::cout << ", ";
						}
					}
				}
				std::cout << "]";
			}
		}
		else
		{
			std::list<int>::iterator agent_it, agent_end;
			agent_it = agents->begin();
			agent_end = agents->end();

			std::cout << (clause_type == ClauseType::POSITIVE ? "[" : "<");
			while(agent_it != agent_end)
			{
				std::cout << *agent_it;
				agent_it++;
				if(agent_it != agent_end)
				{
					std::cout << ", ";
				}
			}
			std::cout << (clause_type == ClauseType::POSITIVE ? "]" : ">");
		}
	}

	if(right->size() > 0)
	{
		std::list<Literal*>* right_list = right->list;
		it = right_list->begin();
		end = right_list->end();
		std::cout << "(";

		while(it != end)
		{
			if(!(*it)->polarity)
			{
				std::cout << "~";
			}
			std::cout << (*it)->identifier;
			it++;
			if(it != end)
			{
				std::cout << " v ";
			}
		}

		std::cout << ")";
	}
	else
	{
		std::cout << "false";
	}

	if(justification != nullptr)
	{
		std::cout << "     [" << justification->input_clause_1;
		if(justification->input_clause_2 != -1)
		{
			std::cout << ", " <<
				justification->input_clause_2;
		}
		std::cout << ", ";
		if(justification->input_clause_2 != -1)
		{
			std::cout << justification->resolved_literal->identifier << ", ";
		}
		std::cout << INFERENCE_RULE_STRINGS[justification->inference_rule] <<
			"]";
	}
	else
	{
		std::cout << "     [Given]";
	}

	std::cout << std::endl;
}

bool Clause::subsumes(Clause* that)
{
	if(that->clause_type == ClauseType::INITIAL ||
		that->clause_type == ClauseType::UNIVERSAL)
	{
		return right->isSubsetOf(that->right);
	}
	else if(clause_type == ClauseType::UNIVERSAL)
	{
		return right->isSubsetOf(that->right) ||
			right->isSubsetOfNegationOf(that->left);
	}
	else
	{
		if(clause_type == that->clause_type)
		{
			return coalitionSubsumes(that) &&
				right->isSubsetOf(that->right) &&
				left->isSubsetOf(that->left);
		}
		else
		{
			return false;
		}
	}
}

bool Clause::initialSubsumes(Clause* that)
{
	if(that->clause_type == ClauseType::INITIAL ||
		that->clause_type == ClauseType::UNIVERSAL)
	{
		return right->isSubsetOf(that->right);
	}
	else if(clause_type == ClauseType::UNIVERSAL)
	{
		return right->isSubsetOf(that->right) ||
			right->isSubsetOfNegationOf(that->left);
	}
	else
	{
		if(clause_type == that->clause_type)
		{
			if(clause_type == ClauseType::POSITIVE)
			{
				return isAgentSubsetOf(that) &&
					right->isSubsetOf(that->right) &&
					left->isSubsetOf(that->left);
			}
			else
			{
				return that->isAgentSubsetOf(this) &&
					right->isSubsetOf(that->right) &&
					left->isSubsetOf(that->left);
			}
		}
		else
		{
			return false;
		}
	}
}

std::list<int>* Clause::relativeComplementAgents(
	Clause* that)
{
	std::list<int>* those_agents = that->agents;
	std::list<int>* new_agents = new std::list<int>();
	std::set_difference(agents->begin(), agents->end(), those_agents->begin(),
		those_agents->end(), std::inserter(*new_agents, new_agents->begin()));
	return new_agents;
}

std::list<int>* Clause::unionAgents(Clause* that)
{
	std::list<int>* those_agents = that->agents;
	std::list<int>* new_agents = new std::list<int>();
	std::set_union(agents->begin(), agents->end(), those_agents->begin(),
		those_agents->end(), std::inserter(*new_agents,
			new_agents->begin()));
	return new_agents;
}

std::list<int>* Clause::intersectionAgents(Clause* that)
{
	std::list<int>* those_agents = that->agents;
	std::list<int>* new_agents = new std::list<int>();
	std::set_intersection(agents->begin(), agents->end(), those_agents->begin(),
		those_agents->end(), std::inserter(*new_agents,
			new_agents->begin()));
	return new_agents;
}

std::pair<std::vector<int>*, const bool>* Clause::mergeCoalitions(Clause* that)
{
	if(coalition_vector == nullptr && that->coalition_vector == nullptr)
	{
		return new std::pair<std::vector<int>*,
			const bool>(nullptr, true);
	}
	std::vector<int>* new_coalition_vector = new std::vector<int>();
	auto this_it = coalition_vector->begin();
	auto this_end = coalition_vector->end();
	auto that_it = that->coalition_vector->begin();
	bool merge_permitted = true;
	int negative = 0;
	while(this_it != this_end)
	{
		int this_move = *this_it;
		int that_move = *that_it;
		if((this_move == that_move) || (that_move == 0 && this_move != 0))
		{
			new_coalition_vector->push_back(this_move);
			// all negative moves must be the same
			if(this_move < 0)
			{
				if(!negative)
				{
					negative = this_move;
				}
				else
				{
					if(this_move != negative)
					{
						return new std::pair<std::vector<int>*,
							const bool>(new_coalition_vector, false);
					}
				}
			}
		}
		else if(this_move == 0 && that_move != 0)
		{
			new_coalition_vector->push_back(that_move);
			// all negative moves must be the same
			if(that_move < 0)
			{
				if(!negative)
				{
					negative = that_move;
				}
				else
				{
					if(that_move != negative)
					{
						return new std::pair<std::vector<int>*,
							const bool>(new_coalition_vector, false);
					}
				}
			}
		}
		else
		{
			return new std::pair<std::vector<int>*,
				const bool>(new_coalition_vector, false);
		}
		this_it++;
		that_it++;
	}
	return new std::pair<std::vector<int>*, const bool>(new_coalition_vector,
		merge_permitted);
}

void Clause::nullifyLeft()
{
	delete left->list;
	left->list = nullptr;
}

/*==============================================================================
	Private Functions
==============================================================================*/
bool Clause::coalitionSubsumes(Clause* that)
{
	auto that_agents_it = that->coalition_vector->begin();
	bool subsumes = true;
	std::for_each(coalition_vector->begin(), coalition_vector->end(),
		[&](int move)
		{
			if(move != 0 and move != *that_agents_it)
			{
				// we cannot subsume
				subsumes = false;
				return;
			}
			that_agents_it++;
		});
	return subsumes;
}

bool Clause::isAgentSubsetOf(Clause* that)
{
	std::list<int>* those = that->agents;
	if(agents->size() == 0)
	{
		return true;
	}
	else if(those->size() == 0)
	{
		return false;
	}
	else if(agents->back() > those->back())
	{
		return false;
	}
	return std::includes(those->begin(), those->end(), agents->begin(),
		agents->end());
}
