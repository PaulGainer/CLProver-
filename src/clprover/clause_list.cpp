/*==============================================================================
	File			: clause_list.cpp
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
#include <algorithm>
#include <functional>
#include <iostream>
#include <set>

#include "clprover/clause_list.h"
#include "clprover/define.h"
#include "clprover/global.h"

#include "data_structures/clause.h"
#include "data_structures/clause_index.h"
#include "data_structures/index_node.h"

void clause_list::sortClauseList(std::list<Clause*>* clause_list)
{
	clause_list->sort([](Clause* this_clause, Clause* that_clause)
	{
		return *this_clause < *that_clause;
	});
}

void clause_list::backwardSubsumption(std::list<Clause*>* clause_list,
	ClauseIndex& clause_index)
{
	std::for_each(clause_list->begin(), clause_list->end(),
		[&](Clause* this_clause)
		{
			std::list<ClauseType>* clause_types = new std::list<ClauseType>();
			std::list<IndexNode*>* filtered_clauses = nullptr;
			std::list<Clause*> removed_clauses;
			int rank = this_clause->getRight()->getMaximalLiteral()->rank;

			// retrieve a list of subsumable candidate clauses from the index
			switch(this_clause->getClauseType())
			{
			case ClauseType::INITIAL:
				clause_types->push_back(ClauseType::INITIAL);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::UNIVERSAL:
				clause_types->push_back(ClauseType::INITIAL);
				clause_types->push_back(ClauseType::UNIVERSAL);
				clause_types->push_back(ClauseType::POSITIVE);
				clause_types->push_back(ClauseType::NEGATIVE);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::POSITIVE:
				clause_types->push_back(ClauseType::POSITIVE);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::NEGATIVE:
				clause_types->push_back(ClauseType::NEGATIVE);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::ALL:
				break;
			}

			std::for_each(filtered_clauses->begin(), filtered_clauses->end(),
				[&](IndexNode* node)
				{
					//IndexNode* node = *filtered_it;
					while(node != nullptr)
					{
						if(this_clause->subsumes(node->clause))
						{
							/* this indexed clause is subsumed so add it to the
							   list of clauses to remove and add it to the
							   archive */
							if(g_verbosity >= V_MAXIMAL)
							{
								std::cout << DIVIDER_2;
								std::cout << "Backward subsumption:\n";
								node->clause->displayClause();
								std::cout << "Subsumed by:\n";
								this_clause->displayClause();
								std::cout << DIVIDER_2;
							}
							g_backward_subsumed_count++;
							g_archive->push_back(node->clause);
							removed_clauses.push_back(node->clause);
						}
						node = node->next;
					}
				});
			// finally remove all of the subsumed clauses from the index
			std::for_each(removed_clauses.begin(), removed_clauses.end(),
				[&](Clause* c)
				{
					clause_index.removeFromIndex(c);
				});
			delete filtered_clauses;
		});
}

void clause_list::backwardSubsumptionGetNext(std::list<Clause*>* clause_list,
	ClauseIndex& clause_index,
	std::set<Clause*, std::function<bool (Clause*, Clause*)>>* i_set,
	std::set<Clause*, std::function<bool (Clause*, Clause*)>>* u_set,
	std::set<Clause*, std::function<bool (Clause*, Clause*)>>* c_set)
{
	std::for_each(clause_list->begin(), clause_list->end(),
		[&](Clause* this_clause)
		{
			std::list<ClauseType>* clause_types = new std::list<ClauseType>();
			std::list<IndexNode*>* filtered_clauses = nullptr;
			std::list<Clause*> removed_clauses;
			int rank = this_clause->getRight()->getMaximalLiteral()->rank;

			// retrieve a list of subsumable candidate clauses from the index
			switch(this_clause->getClauseType())
			{
			case ClauseType::INITIAL:
				clause_types->push_back(ClauseType::INITIAL);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::UNIVERSAL:
				clause_types->push_back(ClauseType::INITIAL);
				clause_types->push_back(ClauseType::UNIVERSAL);
				clause_types->push_back(ClauseType::POSITIVE);
				clause_types->push_back(ClauseType::NEGATIVE);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::POSITIVE:
				clause_types->push_back(ClauseType::POSITIVE);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::NEGATIVE:
				clause_types->push_back(ClauseType::NEGATIVE);
				filtered_clauses = clause_index.filterByMinLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::ALL:
				break;
			}

			std::for_each(filtered_clauses->begin(), filtered_clauses->end(),
				[&](IndexNode* node)
				{
					//IndexNode* node = *filtered_it;
					while(node != nullptr)
					{
						if(this_clause->subsumes(node->clause))
						{
							/* this indexed clause is subsumed so add it to the
							   list of clauses to remove and add it to the
							   archive */
							if(g_verbosity >= V_MAXIMAL)
							{
								std::cout << DIVIDER_2;
								std::cout << "Backward subsumption:\n";
								node->clause->displayClause();
								std::cout << "Subsumed by:\n";
								this_clause->displayClause();
								std::cout << DIVIDER_2;
							}
							g_backward_subsumed_count++;
							g_archive->push_back(node->clause);
							removed_clauses.push_back(node->clause);
							if(node->clause->getClauseType()
								== ClauseType::UNIVERSAL)
							{
								u_set->erase(node->clause);
							}
							else if(node->clause->getClauseType()
								== ClauseType::INITIAL)
							{
								i_set->erase(node->clause);
							}
							else
							{
								c_set->erase(node->clause);
							}
						}
						node = node->next;
					}
				});
			// finally remove all of the subsumed clauses from the index
			std::for_each(removed_clauses.begin(), removed_clauses.end(),
				[&](Clause* c)
				{
					clause_index.removeFromIndex(c);
				});
			delete filtered_clauses;
		});
}

std::list<Clause*>* clause_list::subsumption(std::list<Clause*>* clause_list,
	ClauseIndex& clause_index)
{
	// we will add clauses that are not subsumed to this list
	std::list<Clause*>* not_subsumed = new std::list<Clause*>();
	std::for_each(clause_list->begin(), clause_list->end(),
		[&](Clause* this_clause)
		{
			std::list<ClauseType>* clause_types = new std::list<ClauseType>();
			std::list<IndexNode*>* filtered_clauses = nullptr;
			std::list<IndexNode*>* temp_clause_list;
			int rank = this_clause->getRight()->getMaximalLiteral()->rank;

			/* retrieve a list of candidate clauses that could subsume this
			   clause */
			switch(this_clause->getClauseType())
			{
			case ClauseType::INITIAL:
				clause_types->push_back(ClauseType::INITIAL);
				clause_types->push_back(ClauseType::UNIVERSAL);
				filtered_clauses = clause_index.filterByMaxLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::UNIVERSAL:
				clause_types->push_back(ClauseType::UNIVERSAL);
				filtered_clauses = clause_index.filterByMaxLiteralAndLength(
					rank, this_clause->size(), clause_types);
				break;

			case ClauseType::POSITIVE:
				clause_types->push_back(ClauseType::UNIVERSAL);
				filtered_clauses = clause_index.filterByMaxLiteralAndLength(
					rank, this_clause->getRight()->size(), clause_types);
				clause_types = new std::list<ClauseType>();
				clause_types->push_back(ClauseType::POSITIVE);
				temp_clause_list = clause_index.filterByMaxLiteralAndLength(
					rank, this_clause->size(), clause_types);
				filtered_clauses->splice(filtered_clauses->end(), *temp_clause_list);
				delete temp_clause_list;
				break;

			case ClauseType::NEGATIVE:
				clause_types->push_back(ClauseType::UNIVERSAL);
				filtered_clauses = clause_index.filterByMaxLiteralAndLength(
					rank, this_clause->getRight()->size(), clause_types);
				clause_types = new std::list<ClauseType>();
				clause_types->push_back(ClauseType::NEGATIVE);
				temp_clause_list = clause_index.filterByMaxLiteralAndLength(
					rank, this_clause->size(), clause_types);
				filtered_clauses->splice(filtered_clauses->end(), *temp_clause_list);
				delete temp_clause_list;
				break;

			case ClauseType::ALL:
				break;
			}

			bool subsumed = false;
			std::list<IndexNode*>::iterator filtered_it = filtered_clauses->begin();
			std::list<IndexNode*>::iterator filtered_end = filtered_clauses->end();
			while(!subsumed && filtered_it != filtered_end)
			{
				IndexNode* node = *filtered_it;
				while(!subsumed && node != nullptr)
				{
					if(node->clause->subsumes(this_clause))
					{
						// this clause is subsumed by the indexed clause
						subsumed = true;
						if(g_verbosity >= V_MAXIMAL)
						{
							std::cout << DIVIDER_2;
							std::cout << "Forward subsumption:\n";
							this_clause->displayClause();
							std::cout << "Subsumed by:\n";
							node->clause->displayClause();
							std::cout << DIVIDER_2;
						}
					}
					node = node->next;
				}
				filtered_it++;
			}

			if(!subsumed)
			{
				not_subsumed->push_back(this_clause);
			}
			else
			{
				g_forward_subsumed_count++;
				delete this_clause;
			}

			delete filtered_clauses;
		});

	delete clause_list;
	return not_subsumed;
}

std::list<Clause*>* clause_list::selfSubsumption(std::list<Clause*>* clause_list,
	ClauseIndex& clause_index)
{
	clause_list::sortClauseList(clause_list);
	std::list<Clause*>* not_subsumed = new std::list<Clause*>();
	std::list<Clause*>::iterator clauses_it = clause_list->begin();
	std::list<Clause*>::iterator clauses_end = clause_list->end();
	while(clauses_it != clauses_end)
	{
		Clause* this_clause = *clauses_it;
		std::list<ClauseType>* clause_types = new std::list<ClauseType>();
		std::list<IndexNode*>* filtered_clauses = nullptr;
		std::list<IndexNode*>* temp_clause_list;
		int rank = this_clause->getRight()->getMaximalLiteral()->rank;

		// retrieve a list of candidate clauses that could subsume this clause
		switch(this_clause->getClauseType())
		{
		case ClauseType::INITIAL:
			clause_types->push_back(ClauseType::INITIAL);
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			break;

		case ClauseType::UNIVERSAL:
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			break;

		case ClauseType::POSITIVE:
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->getRight()->size(), clause_types);
			clause_types = new std::list<ClauseType>();
			clause_types->push_back(ClauseType::POSITIVE);
			temp_clause_list = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			filtered_clauses->splice(filtered_clauses->end(), *temp_clause_list);
			delete temp_clause_list;
			break;

		case ClauseType::NEGATIVE:
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->getRight()->size(), clause_types);
			clause_types = new std::list<ClauseType>();
			clause_types->push_back(ClauseType::NEGATIVE);
			temp_clause_list = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			filtered_clauses->splice(filtered_clauses->end(), *temp_clause_list);
			delete temp_clause_list;
			break;

		case ClauseType::ALL:
			break;
		}

		bool subsumed = false;
		std::list<IndexNode*>::iterator filtered_it = filtered_clauses->begin();
		std::list<IndexNode*>::iterator filtered_end = filtered_clauses->end();
		while(!subsumed && filtered_it != filtered_end)
		{
			IndexNode* node = *filtered_it;
			while(!subsumed && node != nullptr)
			{
				if(node->clause->subsumes(this_clause))
				{
					// this clause is subsumed by the indexed clause
					subsumed = true;
					if(g_verbosity >= V_MAXIMAL)
					{
						std::cout << DIVIDER_2;
						std::cout << "Forward subsumption:\n";
						this_clause->displayClause();
						std::cout << "Subsumed by:\n";
						node->clause->displayClause();
						std::cout << DIVIDER_2;
					}
				}
				node = node->next;
			}
			filtered_it++;
		}

		if(!subsumed)
		{
			// this clause was not subsumed so it is added to the index
			not_subsumed->push_back(this_clause);
			clause_index.addToIndex(this_clause);
			clauses_it++;
		}
		else
		{
			/* this clause was subsumed; forward subsumed clauses are discarded
			   so delete it */
			g_forward_subsumed_count++;
			delete *clauses_it;
			clauses_it = clause_list->erase(clauses_it);
		}

		delete filtered_clauses;
	}

	// finally remove all of the clauses from the index
	std::for_each(clause_list->begin(), clause_list->end(),
		[&](Clause* c)
		{
			clause_index.removeFromIndex(c);
		});

	delete clause_list;
	return not_subsumed;
}

std::list<Clause*>* clause_list::initialSelfSubsumption(std::list<Clause*>* clause_list,
	ClauseIndex& clause_index)
{
	clause_list::sortClauseList(clause_list);
	std::list<Clause*>* not_subsumed = new std::list<Clause*>();
	std::list<Clause*>::iterator clauses_it = clause_list->begin();
	std::list<Clause*>::iterator clauses_end = clause_list->end();
	while(clauses_it != clauses_end)
	{
		Clause* this_clause = *clauses_it;
		std::list<ClauseType>* clause_types = new std::list<ClauseType>();
		std::list<IndexNode*>* filtered_clauses = nullptr;
		std::list<IndexNode*>* temp_clause_list;
		int rank = this_clause->getRight()->getMaximalLiteral()->rank;

		// retrieve a list of candidate clauses that could subsume this clause
		switch(this_clause->getClauseType())
		{
		case ClauseType::INITIAL:
			clause_types->push_back(ClauseType::INITIAL);
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			break;

		case ClauseType::UNIVERSAL:
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			break;

		case ClauseType::POSITIVE:
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->getRight()->size(), clause_types);
			clause_types = new std::list<ClauseType>();
			clause_types->push_back(ClauseType::POSITIVE);
			temp_clause_list = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			filtered_clauses->splice(filtered_clauses->end(), *temp_clause_list);
			delete temp_clause_list;
			break;

		case ClauseType::NEGATIVE:
			clause_types->push_back(ClauseType::UNIVERSAL);
			filtered_clauses = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->getRight()->size(), clause_types);
			clause_types = new std::list<ClauseType>();
			clause_types->push_back(ClauseType::NEGATIVE);
			temp_clause_list = clause_index.filterByMaxLiteralAndLength(
				rank, this_clause->size(), clause_types);
			filtered_clauses->splice(filtered_clauses->end(), *temp_clause_list);
			delete temp_clause_list;
			break;

		case ClauseType::ALL:
			break;
		}

		bool subsumed = false;
		std::list<IndexNode*>::iterator filtered_it = filtered_clauses->begin();
		std::list<IndexNode*>::iterator filtered_end = filtered_clauses->end();
		while(!subsumed && filtered_it != filtered_end)
		{
			IndexNode* node = *filtered_it;
			while(!subsumed && node != nullptr)
			{
				if(node->clause->initialSubsumes(this_clause))
				{
					// this clause is subsumed by the indexed clause
					subsumed = true;
					if(g_verbosity >= V_MAXIMAL)
					{
						std::cout << DIVIDER_2;
						std::cout << "Forward subsumption:\n";
						this_clause->displayClause();
						std::cout << "Subsumed by:\n";
						node->clause->displayClause();
						std::cout << DIVIDER_2;
					}
				}
				node = node->next;
			}
			filtered_it++;
		}

		if(!subsumed)
		{
			// this clause was not subsumed so it is added to the index
			not_subsumed->push_back(this_clause);
			clause_index.addToIndex(this_clause);
			clauses_it++;
		}
		else
		{
			/* this clause was subsumed; forward subsumed clauses are discarded
				so delete it */
			g_forward_subsumed_count++;
			delete *clauses_it;
			clauses_it = clause_list->erase(clauses_it);
		}

		delete filtered_clauses;
	}

	// finally remove all of the clauses from the index
	std::for_each(clause_list->begin(), clause_list->end(),
		[&](Clause* c)
		{
			clause_index.removeFromIndex(c);
		});

	delete clause_list;
	return not_subsumed;
}
