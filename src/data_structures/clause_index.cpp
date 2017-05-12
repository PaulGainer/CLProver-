/*==============================================================================
	File			: clause_index.cpp
	Author			: Paul Gainer
	Created			: 29/06/2014
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
#include <cstdarg>

#include "clprover/define.h"
#include "clprover/global.h"

#include "data_structures/clause.h"
#include "data_structures/clause_index.h"
#include "data_structures/index_node.h"
#include "data_structures/justification.h"

#include "enums/clause_type.h"

/*==============================================================================
	Private Structs
==============================================================================*/
struct ClauseIndex::InternalNode
{
	int count = 0;
	InternalNode** next_node = nullptr;
};

struct ClauseIndex::InternalNodeTerminal: InternalNode
{
	IndexNode** leaves = nullptr;
};

/*==============================================================================
	Constructors/Deconstructor
==============================================================================*/
ClauseIndex::ClauseIndex(const int F_COUNT, ...):
	feature_count(F_COUNT)
{
	max_feature_values = new int[feature_count];
	va_list arguments;
	va_start(arguments, F_COUNT);
	for(int i = 0; i < feature_count; i++)
	{
		max_feature_values[i] = va_arg(arguments, int);
	}
	va_end(arguments);
	root = buildNextLevel(0, nullptr);
}


ClauseIndex::~ClauseIndex()
{
	deleteNextLevel(0, root);
	delete[] max_feature_values;
}

/*==============================================================================
	Public Functions
==============================================================================*/
void ClauseIndex::addToIndex(Clause* clause)
{
	int* feature_vector = clause->calculateFeatures();

	InternalNode** node = root;
	int i = 0;
	// traverse the tree until the penultimate level is reached
	while(i < feature_count - 2)
	{
		node[feature_vector[i]]->count++;
		node = node[feature_vector[i]]->next_node;
		i++;
	}
	node[feature_vector[i]]->count++;
	InternalNodeTerminal* new_terminal_node =
		static_cast<InternalNodeTerminal*>(node[feature_vector[i++]]);
	int index = feature_vector[i];
	IndexNode** leaves = new_terminal_node->leaves;
	if(leaves[index] == nullptr)
	{
		// there was nothing stored here so start a new list
		leaves[index] = new IndexNode(clause, nullptr, nullptr);
		// update the maximum clause length if needed
		if(clause->size() > max_clause_length)
		{
			max_clause_length = clause->size();
		}
	}
	else
	{
		// store the clause in a new IndexNode at the start of the list
		IndexNode* new_node = new IndexNode(clause, leaves[index], nullptr);
		leaves[index]->prev = new_node;
		leaves[index] = new_node;
	}
	clause->index_node = leaves[index];
	num_clauses++;
	delete[] feature_vector;
}

void ClauseIndex::removeFromIndex(Clause* clause)
{
	if(clause->index_node == nullptr)
	{
		// the clause is not stored in an index so nothing to do here
		return;
	}
	int* feature_vector = clause->calculateFeatures();
	InternalNode** node = root;
	int i = 0;
	// traverse the tree until the penultimate level is reached
	while(i < feature_count - 2)
	{
		node[feature_vector[i]]->count--;
		node = node[feature_vector[i]]->next_node;
		i++;
	}
	node[feature_vector[i]]->count--;
	InternalNodeTerminal* new_terminal_node =
		static_cast<InternalNodeTerminal*>(node[feature_vector[i++]]);
	int index = feature_vector[i];
	IndexNode** leaves = new_terminal_node->leaves;
	IndexNode* prev = clause->index_node->prev;
	IndexNode* next = clause->index_node->next;
	if(prev == nullptr)
	{
		if(next == nullptr)
		{
			// the only node in the list
			leaves[index] = nullptr;
		}
		else
		{
			// the first node in the list
			next->prev = nullptr;
			leaves[index] = next;
		}
	}
	else
	{
		if(next == nullptr)
		{
			// the last node in the list
			prev->next = nullptr;
		}
		else
		{
			// in between two nodes in the list
			prev->next = next;
			next->prev = prev;
		}
	}
	delete clause->index_node;
	clause->index_node = nullptr;
	num_clauses--;
	delete[] feature_vector;
}

void ClauseIndex::displayIndex()
{
	std::list<Clause*>* all_clauses = new std::list<Clause*>();
	displayNextFeature(root, 0, all_clauses);
	// sort all of the clauses by identifier...
	all_clauses->sort([](Clause* this_clause, Clause* that_clause)
		{
			return this_clause->identifier < that_clause->identifier;
		});
	// ...then display them
	std::for_each(all_clauses->begin(), all_clauses->end(), [](Clause* clause)
		{
			clause->displayClause();
		});
	delete all_clauses;
}

std::list<IndexNode*>* ClauseIndex::filterByMaxLiteral(int literal_rank,
	std::list<ClauseType>* clause_types)
{
	std::list<IndexNode*>* new_clause_list = new std::list<IndexNode*>();
	if(clause_types == nullptr)
	{
		// we want clauses of all types
		int* feature_vector = new int[3] {-1, literal_rank, -1};
		getClauseList(feature_vector, new_clause_list);
		delete[] feature_vector;
		return new_clause_list;
	}
	else
	{
		// we only want certain types of clause
		int* feature_vector = new int[3] {-1, literal_rank, -1};
		std::for_each(clause_types->begin(), clause_types->end(),
			[&](ClauseType type)
			{
				feature_vector[0] = type;
				getClauseList(feature_vector, new_clause_list);
			});
		delete[] feature_vector;
		delete clause_types;
		return new_clause_list;
	}
}

std::list<IndexNode*>* ClauseIndex::filterByMaxLiteralAndLength(
	int rank, int length, std::list<ClauseType>* clause_types)
{
	std::list<IndexNode*>* new_clause_list = new std::list<IndexNode*>();
	int* feature_vector = new int[3] {-1, -1, -1};
	for(int i = 0; i <= length; i++)
	{
		feature_vector[2] = i;
		std::for_each(clause_types->begin(), clause_types->end(),
			[&](ClauseType type)
			{
				feature_vector[0] = type;
				for(int j = 0; j <= rank; j++)
				{
					feature_vector[1] = j;
					getClauseList(feature_vector, new_clause_list);
				}
			});
	}
	delete[] feature_vector;
	delete clause_types;
	return new_clause_list;
}

std::list<IndexNode*>* ClauseIndex::filterByMinLiteralAndLength(
	int rank, int length, std::list<ClauseType>* clause_types)
{
	std::list<IndexNode*>* new_clause_list = new std::list<IndexNode*>();
	int* feature_vector = new int[3] {-1, -1, -1};
	// don't iterate past the length of the largest stored clause
	for(int i = length; i <= max_clause_length; i++)
	{
		feature_vector[2] = i;
		std::for_each(clause_types->begin(), clause_types->end(),
			[&](ClauseType type)
			{
				feature_vector[0] = type;
				for(unsigned int j = rank; j <= g_literals->size(); j++)
				{
					feature_vector[1] = j;
					getClauseList(feature_vector, new_clause_list);
				}
			});
	}
	delete[] feature_vector;
	delete clause_types;
	return new_clause_list;
}

Clause* ClauseIndex::getNextSmallestClause()
{
	if(num_clauses == 0)
	{
		// the index is empty
		return nullptr;
	}
	std::list<IndexNode*>* clauses = new std::list<IndexNode*>();
	int num_literals = g_literals->size();
	// we want the smallest clauses first
	for(int i = 1; i <= num_literals; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(root[j]->count > 0)
			{
				for(int k = 1; k <= num_literals; k++)
				{
					if(root[j]->next_node[k]->count > 0)
					{
						int* feature_vector = new int[3] {j, k, i};
						getClauseList(feature_vector, clauses);
						if(clauses->size() > 0)
						{
							// clauses were retrieved so return the first one
							Clause* c = clauses->front()->clause;
							delete[] feature_vector;
							delete clauses;
							return c;
						}
						delete[] feature_vector;
					}
				}
			}
		}
	}
	delete clauses;
	return nullptr;
}

std::list<Clause*>* ClauseIndex::resolve(Clause* clause)
{
	std::list<Clause*>* resolved_clauses = new std::list<Clause*>();
	std::list<IndexNode*>* filtered_clauses = nullptr;
	std::list<IndexNode*>::iterator filtered_it;
	std::list<IndexNode*>::iterator filtered_end;
	Literal* max_literal = clause->right->getMaximalLiteral();
	std::list<ClauseType>* clause_types = new std::list<ClauseType>();
	Clause* resolvent;

	switch(clause->clause_type)
	{
	case ClauseType::INITIAL:
		clause_types->push_back(ClauseType::INITIAL);
		clause_types->push_back(ClauseType::UNIVERSAL);
		// get all the clauses having as maximal literal the complement of
		// the maximal literal of the given clause
		filtered_clauses =
			filterByMaxLiteral(max_literal->complementary_literal->rank,
				clause_types);
		filtered_it = filtered_clauses->begin();
		filtered_end = filtered_clauses->end();
		while(filtered_it != filtered_end)
		{
			IndexNode* node = *filtered_it;
			while(node != nullptr)
			{
				Clause* index_clause = node->clause;
				std::pair<LiteralList*, const bool>* pair =
					LiteralList::unionLiterals(
						clause->right->removeMaximalLiteral(),
						index_clause->right->removeMaximalLiteral());
				if(!pair->second)
				{
					// IRES1 has been applied
					g_inference_count_ires1++;
					resolvent = new Clause(new LiteralList(),
						new std::list<int>(), pair->first, ClauseType::INITIAL,
						new Justification(clause->identifier,
							index_clause->identifier, max_literal,
							InferenceRule::IRES1), nullptr);
					if(resolvent->size() == 0)
					{
						// a contradiction has been derived
						g_contradiction = resolvent;
						delete pair;
						delete filtered_clauses;
						return resolved_clauses;
					}
					else
					{
						resolved_clauses->push_back(resolvent);
					}
				}
				else
				{
					// a tautology was derived
					g_inference_count_tautology++;
					if(g_verbosity >= V_MAXIMAL)
					{
						displayTautology(clause, index_clause);
					}
					delete pair->first;
				}
				delete pair;
				node = node->next;
			}
			filtered_it++;
		}
		break;

	case ClauseType::UNIVERSAL:
		clause_types->push_back(ClauseType::INITIAL);
		clause_types->push_back(ClauseType::UNIVERSAL);
		clause_types->push_back(ClauseType::POSITIVE);
		clause_types->push_back(ClauseType::NEGATIVE);
		// get all the clauses having as maximal literal the complement of
		// the maximal literal of the given clause
		filtered_clauses =
			filterByMaxLiteral(max_literal->complementary_literal->rank,
				clause_types);
		filtered_it = filtered_clauses->begin();
		filtered_end = filtered_clauses->end();
		while(filtered_it != filtered_end)
		{
			IndexNode* node = *filtered_it;
			while(node != nullptr)
			{
				Clause* index_clause = node->clause;
				std::pair<LiteralList*, const bool>* pair =
					LiteralList::unionLiterals(
						clause->right->removeMaximalLiteral(),
						index_clause->right->removeMaximalLiteral());
				if(!pair->second)
				{
					switch(index_clause->clause_type)
					{
					case ClauseType::INITIAL:
						// IRES1 has been applied
						g_inference_count_ires1++;
						resolvent = new Clause(
							new LiteralList(), new std::list<int>(),
							pair->first, ClauseType::INITIAL,
							new Justification(clause->identifier,
								index_clause->identifier, max_literal,
								InferenceRule::IRES1), nullptr);
						if(resolvent->size() == 0)
						{
							// a contradiction has been derived
							g_contradiction = resolvent;
							delete pair;
							delete filtered_clauses;
							return resolved_clauses;
						}
						else
						{
							resolved_clauses->push_back(resolvent);
						}
						break;

					case ClauseType::UNIVERSAL:
						// GRES1 has been applied
						g_inference_count_gres1++;
						resolvent = new Clause(
							new LiteralList(), new std::list<int>(),
							pair->first, ClauseType::UNIVERSAL,
							new Justification(clause->identifier,
								index_clause->identifier, max_literal,
								InferenceRule::GRES1), nullptr);
						if(resolvent->size() == 0)
						{
							// a contradiction has been derived
							g_contradiction = resolvent;
							delete pair;
							delete filtered_clauses;
							return resolved_clauses;
						}
						else
						{
							resolved_clauses->push_back(resolvent);
						}
						break;

					case ClauseType::POSITIVE:
						// CRES2 has been applied
						g_inference_count_cres2++;
						resolvent = new Clause(
							new LiteralList(*(index_clause->left)),
							new std::list<int>(*(index_clause->agents)),
							pair->first, ClauseType::POSITIVE,
							new Justification(clause->identifier,
								index_clause->identifier, max_literal,
								InferenceRule::CRES2),
							new std::vector<int>(
								*(index_clause->coalition_vector)));
						if(resolvent->size() == 0)
						{
							// a contradiction has been derived
							g_contradiction = resolvent;
							delete pair;
							delete filtered_clauses;
							return resolved_clauses;
						}
						else
						{
							resolved_clauses->push_back(resolvent);
						}
						break;

					case ClauseType::NEGATIVE:
						// CRES4 has been applied
						g_inference_count_cres4++;
						resolvent = new Clause(
							new LiteralList(*(index_clause->left)),
							new std::list<int>(*(index_clause->agents)),
							pair->first, ClauseType::NEGATIVE,
							new Justification(clause->identifier,
								index_clause->identifier, max_literal,
								InferenceRule::CRES4),
							new std::vector<int>(
								*(index_clause->coalition_vector)));
						if(resolvent->size() == 0)
						{
							// a contradiction has been derived
							g_contradiction = resolvent;
							delete pair;
							delete filtered_clauses;
							return resolved_clauses;
						}
						else
						{
							resolved_clauses->push_back(resolvent);
						}
						break;

					case ClauseType::ALL:
						break;
					}
				}
				else
				{
					// a tautology was derived
					g_inference_count_tautology++;
					if(g_verbosity >= V_MAXIMAL)
					{
						displayTautology(clause, index_clause);
					}
					delete pair->first;
				}
				delete pair;
				node = node->next;
			}
			filtered_it++;
		}
		break;

	case ClauseType::POSITIVE: case ClauseType::NEGATIVE:
		clause_types->push_back(ClauseType::UNIVERSAL);
		clause_types->push_back(ClauseType::POSITIVE);
		clause_types->push_back(ClauseType::NEGATIVE);
		filtered_clauses =
			filterByMaxLiteral(max_literal->complementary_literal->rank,
				clause_types);
		// get all the clauses having as maximal literal the complement of
		// the maximal literal of the given clause
		filtered_it = filtered_clauses->begin();
		filtered_end = filtered_clauses->end();
		while(filtered_it != filtered_end)
		{
			IndexNode* node = *filtered_it;
			while(node != nullptr)
			{
				Clause* index_clause = node->clause;
				std::pair<LiteralList*, const bool>* pair =
					LiteralList::unionLiterals(
						clause->right->removeMaximalLiteral(),
						index_clause->right->removeMaximalLiteral());
				if(!pair->second)
				{
					if(index_clause->clause_type == ClauseType::UNIVERSAL)
					{
						if(clause->clause_type == ClauseType::POSITIVE)
						{
							// CRES2 has been applied
							g_inference_count_cres2++;
							resolvent =	new Clause(
								new LiteralList(*(clause->left)),
								new std::list<int>(*clause->agents),
								pair->first,
								ClauseType::POSITIVE,
								new Justification(clause->identifier,
									index_clause->identifier, max_literal,
									InferenceRule::CRES2),
								new std::vector<int>(*(
									clause->coalition_vector)));
							if(resolvent->size() == 0)
							{
								// a contradiction has been derived
								g_contradiction = resolvent;
								delete pair;
								delete filtered_clauses;
								return resolved_clauses;
							}
							else
							{
								resolved_clauses->push_back(resolvent);
							}
						}
						else
						{
							// CRES2 has been applied
							g_inference_count_cres2++;
							resolvent = new Clause(
								new LiteralList(*(clause->left)),
								new std::list<int>(*clause->agents),
								pair->first,
								ClauseType::NEGATIVE,
								new Justification(clause->identifier,
									index_clause->identifier, max_literal,
									InferenceRule::CRES4),
								new std::vector<int>(
									*(clause->coalition_vector)));
							if(resolvent->size() == 0)
							{
								// a contradiction has been derived
								g_contradiction = resolvent;
								delete pair;
								delete filtered_clauses;
								return resolved_clauses;
							}
							else
							{
								resolved_clauses->push_back(resolvent);
							}
						}
					}
					else
					{
						std::pair<LiteralList*, const bool>* pair_left =
							LiteralList::unionLiterals(
								new LiteralList(*(clause->left)),
								new LiteralList(*(index_clause->left)));
						if(!pair_left->second)
						{
							std::pair<std::vector<int>*,const bool>* pair_agents =
								clause->mergeCoalitions(index_clause);
							if(pair_agents->second)
							{
								if(clause->clause_type == ClauseType::POSITIVE)
								{
									if(index_clause->clause_type ==
										ClauseType::POSITIVE)
									{
										// CRES1 has been applied
										g_inference_count_cres1++;
										resolvent = new Clause(
											pair_left->first,
											clause->unionAgents(index_clause),
											pair->first, ClauseType::POSITIVE,
											new Justification(
												clause->identifier,
												index_clause->identifier,
												max_literal,
												InferenceRule::CRES1),
											pair_agents->first);
										if(resolvent->size() == 0)
										{
											// a contradiction has been derived
											g_contradiction = resolvent;
											delete pair_agents;
											delete pair_left;
											delete pair;
											delete filtered_clauses;
											return resolved_clauses;
										}
										else
										{
											resolved_clauses->push_back(
												resolvent);
										}
									}
									else
									{
										// CRES3 has been applied
										g_inference_count_cres3++;
										resolvent = new Clause(
											pair_left->first,
											index_clause->
												relativeComplementAgents(clause),
											pair->first,
											ClauseType::NEGATIVE,
											new Justification(
												clause->identifier,
												index_clause->identifier,
												max_literal,
												InferenceRule::CRES3),
											pair_agents->first);
										if(resolvent->size() == 0)
										{
											// a contradiction has been derived
											g_contradiction = resolvent;
											delete pair_agents;
											delete pair_left;
											delete pair;
											delete filtered_clauses;
											return resolved_clauses;
										}
										else
										{
											resolved_clauses->push_back(
												resolvent);
										}
									}
								}
								else
								{
									if(index_clause->clause_type ==
										ClauseType::POSITIVE)
									{
										// CRES3 has been applied
										g_inference_count_cres3++;
										resolvent = new Clause(
										pair_left->first,
										clause->relativeComplementAgents(
											index_clause),pair->first,
											ClauseType::NEGATIVE,
											new Justification(
												clause->identifier,
												index_clause->identifier,
												max_literal,
												InferenceRule::CRES3),
											pair_agents->first);
										if(resolvent->size() == 0)
										{
											// a contradiction has been derived
											g_contradiction = resolvent;
											delete pair_agents;
											delete pair_left;
											delete pair;
											delete filtered_clauses;
											return resolved_clauses;
										}
										else
										{
											resolved_clauses->push_back(
												resolvent);
										}
									}
									else
									{
										// CRES5 has been applied
										g_inference_count_cres5++;
										resolvent = new Clause(
											pair_left->first,
											clause->intersectionAgents(
												index_clause),
											pair->first,
											ClauseType::NEGATIVE,
											new Justification(
												clause->identifier,
												index_clause->identifier,
												max_literal,
												InferenceRule::CRES5),
											pair_agents->first);
										if(resolvent->size() == 0)
										{
											// a contradiction has been derived
											g_contradiction = resolvent;
											delete pair_agents;
											delete pair_left;
											delete pair;
											delete filtered_clauses;
											return resolved_clauses;
										}
										else
										{
											resolved_clauses->push_back(resolvent);
										}
									}
								}
							}
							else
							{
								delete pair->first;
								delete pair_left->first;
								delete pair_agents->first;
							}
							delete pair_agents;
						}
						else
						{
							// a tautology was derived
							g_inference_count_tautology++;
							if(g_verbosity >= V_MAXIMAL)
							{
								displayTautology(clause, index_clause);
							}

							delete pair->first;
							delete pair_left->first;
						}
						delete pair_left;
					}
				}
				else
				{
					// a tautology was derived
					g_inference_count_tautology++;
					if(g_verbosity >= V_MAXIMAL)
					{
						displayTautology(clause, index_clause);
					}
					delete pair->first;
				}
				delete pair;
				node = node->next;
			}
			filtered_it++;
		}
		break;

		case ClauseType::ALL:
		break;
	}

	delete filtered_clauses;
	return resolved_clauses;
}

/*==============================================================================
	Private Functions
==============================================================================*/
ClauseIndex::InternalNode** ClauseIndex::buildNextLevel(int feature,
	InternalNode* previous_node)
{
	InternalNode** this_node =
		new InternalNode*[max_feature_values[feature]];
	if(feature == feature_count - 2)
	{
		// this is the penultimate level so build the final level of internal
		// nodes
		for(int i = 0; i < max_feature_values[feature]; i++)
		{
			InternalNodeTerminal* new_terminal_node = new InternalNodeTerminal;
			IndexNode** index_node_array =
				new IndexNode*[max_feature_values[feature + 1]];
			// build the last level of leaf nodes
			for(int j = 0; j < max_feature_values[feature + 1]; j++)
			{
				index_node_array[j] = nullptr;
			}
			new_terminal_node->leaves = index_node_array;
			this_node[i] = new_terminal_node;
		}
	}
	else
	{
		// build the next level of nodes
		for(int i = 0; i < max_feature_values[feature]; i++)
		{
			InternalNode* new_node = new InternalNode;
			new_node->next_node = buildNextLevel(feature + 1, this_node[i]);
			this_node[i] = new_node;
		}
	}
	return this_node;
}

void ClauseIndex::deleteNextLevel(int feature, InternalNode** node)
{
	if(feature == feature_count - 2)
	{
		// this is the penultimate level so remove the final level of internal
		// nodes
		for(int i = 0; i < max_feature_values[feature]; i++)
		{
			InternalNodeTerminal* terminal_node =
				static_cast<InternalNodeTerminal*>(node[i]);
			IndexNode** leaves = terminal_node->leaves;
			// remove the level of leaf nodes
			for(int j = 0; j < max_feature_values[feature_count - 1]; j++)
			{
				IndexNode* node = leaves[j];
				if(node != nullptr)
				{
					while(node != nullptr)
					{
						IndexNode* temp = node;
						node = node->next;
						delete temp->clause;
						delete temp;
					}
				}
			}
			delete[] leaves;
			delete node[i];
		}
		delete[] node;
	}
	else
	{
		// remove the next level of nodes
		for(int i = 0; i < max_feature_values[feature]; i++)
		{
			deleteNextLevel(feature + 1, node[i]->next_node);
			delete node[i];
		}
		delete[] node;
	}
}

void ClauseIndex::getClauseList(int* feature_vector,
	std::list<IndexNode*>* new_clause_list)
{
	getClauseListNextFeature(root, 0, feature_vector,
		new_clause_list);
}

void ClauseIndex::getClauseListNextFeature(InternalNode** node,
	int feature, int* feature_vector, std::list<IndexNode*>* new_clause_list)
{
	if(feature == feature_count - 2)
	{
		// this is the penultimate layer of the index tree
		if(feature_vector[feature] == -1)
		{
			for(int i = 0; i < max_feature_values[feature]; i++)
			{
				if(node[i]->count > 0)
				{
					InternalNodeTerminal* terminal_node =
						static_cast<InternalNodeTerminal*>(node[i]);
					if(feature_vector[feature + 1] == -1)
					{
						for(int j = 0; j < max_feature_values[feature + 1]; j++)
						{
							// add all of the clauses stored here to the list
							IndexNode* node = terminal_node->leaves[j];
							if(node != nullptr)
							{
								new_clause_list->push_back(node);
							}
						}
					}
					else
					{
						// add all of the clauses stored here to the list
						IndexNode* node =
							terminal_node->leaves[feature_vector[feature + 1]];
						if(node != nullptr)
						{
							new_clause_list->push_back(node);
						}
					}
				}
			}
			return;
		}
		else
		{
			if(node[feature_vector[feature]]->count > 0)
			{
				InternalNodeTerminal* terminal_node =
					static_cast<InternalNodeTerminal*>(node[feature_vector[feature]]);
				if(feature_vector[feature + 1] == -1)
				{
					for(int j = 0; j < max_feature_values[feature + 1]; j++)
					{
						// add all of the clauses stored here to the list
						IndexNode* node = terminal_node->leaves[j];
						if(node != nullptr)
						{
							new_clause_list->push_back(node);
						}
					}
				}
				else
				{
					// add all of the clauses stored here to the list
					IndexNode* node =
						terminal_node->leaves[feature_vector[feature + 1]];
					if(node != nullptr)
					{
						new_clause_list->push_back(node);
					}
				}
			}
		}
	}
	else if(feature_vector[feature] == -1)
	{
		// traverse all children of this node
		for(int i = 0; i < max_feature_values[feature]; i++)
		{
			getClauseListNextFeature(node[i]->next_node,
				feature + 1, feature_vector, new_clause_list);
		}
	}
	else
	{
		// traverse the tree
		getClauseListNextFeature(node[feature_vector[feature]]->next_node,
			feature + 1, feature_vector, new_clause_list);
	}
}

void ClauseIndex::displayNextFeature(InternalNode** node, int feature,
	std::list<Clause*>* clauses)
{
	if(feature == feature_count - 2)
	{
		for(int i = 0; i < max_feature_values[feature]; i++)
		{
			if(node[i]->count > 0)
			{
				InternalNodeTerminal* terminal_node =
					static_cast<InternalNodeTerminal*>(node[i]);
				for(int j = 0; j < max_feature_values[feature + 1]; j++)
				{
					IndexNode* node = terminal_node->leaves[j];
					if(node != nullptr)
					{
						while(node != nullptr)
						{
							clauses->push_back(node->clause);
							node = node->next;
						}
					}
				}
			}
		}
		return;
	}
	for(int i = 0; i < max_feature_values[feature]; i++)
	{
		if(node[i]->count > 0)
		{
			displayNextFeature(node[i]->next_node, feature + 1, clauses);
		}
	}
}

void ClauseIndex::displayTautology(Clause* clause_1, Clause* clause_2)
{
	std::cout << DIVIDER_2;
	std::cout << "Tautology derived by resolving:\n";
	clause_1->displayClause();
	std::cout << "and:\n";
	clause_2->displayClause();
	std::cout << DIVIDER_2;
}
