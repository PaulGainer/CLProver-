/*==============================================================================
	CLProver++ v1.0.3

	An automated theorem prover for coalition logic. Accepts as input a set of
	DSNF_CL^MV formulae and outputs satisfiability.

	File			: clprover.cpp
	Author			: Paul Gainer
	Created			: 16/06/2014
	Last modified	: 18/04/2015
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
#include <ctime>
#include <climits>
#include <iostream>
#include <queue>
#include <set>

#include "clprover/clause_list.h"
#include "clprover/define.h"
#include "clprover/global.h"
#include "clprover/parsing.h"

#include "command_line_parser/command_line_parser.h"
#include "command_line_parser/parser_option_type.h"

#include "data_structures/clause.h"
#include "data_structures/clause_index.h"
#include "data_structures/justification.h"
#include "data_structures/list_literal_iterator_pair.h"
#include "data_structures/literal.h"

/*==============================================================================
	Function Prototypes
==============================================================================*/
void resolution(std::list<Clause*>*);
void unitPropagation(std::list<Clause*>*);
std::list<Clause*>* propagateUnitClause(std::list<Clause*>*, Clause*);
void purityDeletion(std::list<Clause*>*);
void cleanup();
void displayArchive();
void displayTitle(const std::string&);

/*--------------------------------------------------------------------------
	main
--------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
	// the file containing the DSNF_CL^MV clauses
	std::string input_file_name;
	// the optional file defining the literal ordering
	std::string ordering_file_name = "";

	/* set up a new command line parser, an input file is required but all
	   other features are optional; defaults are forward subsumption enabled
	   and output satisfiability and clauses, all other features are disabled */
	CommandLineParser parser(EXEC_NAME, EXEC_VERSION,
		CommandLineParser::PARAMETER_SORT, argc, argv);
	parser.addBoolOption("-u",
		std::string("disable/enable unit propagation\n") +
		std::string("\t\t\tdefault: disabled\n"), CommandLineParser::OPTIONAL,
		&g_unit_propagation);
	parser.addBoolOption("-p",
		std::string("disable/enable purity deletion\n") +
		std::string("\t\t\tdefault: disabled\n"), CommandLineParser::OPTIONAL,
		&g_purity_deletion);
	parser.addBoolOption("-f",
		std::string("disable/enable forward subsumption\n") +
		std::string("\t\t\tdefault: enabled\n"), CommandLineParser::OPTIONAL,
		&g_forward_subsumption);
	parser.addBoolOption("-b",
		std::string("disable/enable backward subsumption\n") +
		std::string("\t\t\tdefault: disabled\n"), CommandLineParser::OPTIONAL,
		&g_backward_subsumption);
	parser.addBoolOption("-c",
		std::string("display modality as vector\n") +
		std::string("\t\t\tdefault: disabled\n"), CommandLineParser::OPTIONAL,
		&g_display_modality_as_vector);
	parser.addBoolOption("-x",
		std::string("display extra inference information\n") +
		std::string("\t\t\tdefault: disabled\n"), CommandLineParser::OPTIONAL,
		&g_display_extra_inference_information);
	parser.addIntRangeOption("-v",
		std::string("set output verbosity\n") +
		std::string("\t\t\t0: output satisfiability\n") +
		std::string("\t\t\t1: output satisfiability and clauses\n") +
		std::string("\t\t\t2: output satisfiability, clauses and derivation\n") +
		std::string("\t\t\t3: debug level output\n") +
		std::string("\t\t\tdefault: output satisfiability and clauses\n"),
		CommandLineParser::OPTIONAL, &g_verbosity, V_MINIMAL, V_DEBUG);
	parser.addIntRangeOption("-h",
		std::string("select heuristic\n") +
		std::string("\t\t\t0: select smallest clause\n") +
		std::string("\t\t\t1: select clause by type\n") +
		std::string("\t\t\tdefault: select smallest clause\n"),
		CommandLineParser::OPTIONAL, &g_heuristic, GET_NEXT_SMALLEST, GET_NEXT);
	parser.addIntRangeOption("-a",
		std::string("set number of agents\n"),
		CommandLineParser::OPTIONAL, &g_num_agents, 1, INT_MAX);
	parser.addStringOption("-i", "set input file\n",
		CommandLineParser::REQUIRED, &input_file_name, "file");
	parser.addStringOption("-o", "set literal ordering file\n",
		CommandLineParser::OPTIONAL, &ordering_file_name, "file");
	if(argc == 1)
	{
		// no arguments so display some info
		parser.displayUsageInformation();
		return 0;
	}
	if(!parser.parse())
	{
		// there was a parse error
		return 0;
	}

	// begin the timer
	clock_t begin = clock();

	if(ordering_file_name != "")
	{
		/* a literal ordering has been defined so we will decrement literal
		   ranks, starting with INT_MAX */
		g_increment_literal_ranks = false;
		Literal::next_literal_rank = INT_MAX;
		if(!parseOrderingFile(ordering_file_name))
		{
			cleanup();
			return 0;
		}
	}
	else
	{
		/* a literal ordering has not been defined so we will increment
		   literal ranks, starting with 1 */
		Literal::next_literal_rank = 1;
	}

	// parse the input file
	std::list<Clause*>* parsed_clauses = parseInputFile(input_file_name);

	if(parsed_clauses == nullptr)
	{
		// there was a parse error so clean up and exit
		cleanup();
		return 0;
	}
	else if(ordering_file_name != "")
	{
		// renumber the clauses so that the lowest clause rank is 1
		int sub = INT_MAX - g_literals->size();
		std::for_each(g_literals->begin(), g_literals->end(),
			[&](Literal* l)
			{
				l->rank -= sub;
			});
	}

	if(!parsed_clauses->empty())
	{
		if(g_verbosity >= V_MAXIMAL)
		{
			// display the parsed clauses
			displayTitle("Parsed Clauses");
			std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
				[](Clause* c)
				{
					c->displayClause();
				});
		}

		if(g_verbosity == V_DEBUG)
		{
			// display the parsed literals
			displayTitle("Parsed Literals");
			std::for_each(g_literals->begin(), g_literals->end(),
				[](Literal* l)
				{
					l->displayLiteral();
				});
		}

		if(g_verbosity >= V_MAXIMAL)
		{
			// display the literal ordering
			displayTitle("Literal Ordering");
			if(ordering_file_name != "")
			{
				/* no ordering was defined, clauses will be ordered by
				   increasing identifiers */
				int size = g_literals->size() - 1;
				for(int i = 0; i <= size; i++)
				{
					std::cout << (g_literals->at(i)->polarity ? "" : "~") <<
						g_literals->at(i)->identifier;
					if(i < size)
					{
						std::cout << " > ";
					}
				}
				std::cout << std::endl;
			}
			else
			{
				/* an ordering was defined, clauses will be ordered by
				   decreasing identifiers */
				int size = g_literals->size() - 1;
				for(int i = size; i >= 0; i--)
				{
					std::cout << (g_literals->at(i)->polarity ? "" : "~") <<
						g_literals->at(i)->identifier;
					if(i > 0)
					{
						std::cout << " > ";
					}
				}
				std::cout << std::endl;
			}
		}
	}
	else
	{
		std::cout << "No clauses were parsed\n";
		cleanup();
		return 0;
	}

	resolution(parsed_clauses);

	if(g_display_extra_inference_information)
	{
		std::cout << "Applications of IRES1: " << g_inference_count_ires1 <<
			std::endl;
		std::cout << "Applications of GRES1: " << g_inference_count_gres1 <<
			std::endl;
		std::cout << "Applications of CRES1: " << g_inference_count_cres1 <<
			std::endl;
		std::cout << "Applications of CRES2: " << g_inference_count_cres2 <<
			std::endl;
		std::cout << "Applications of CRES3: " << g_inference_count_cres3 <<
			std::endl;
		std::cout << "Applications of CRES4: " << g_inference_count_cres4 <<
			std::endl;
		std::cout << "Applications of CRES5: " << g_inference_count_cres5 <<
			std::endl;
		std::cout << "Tautologies derived: " << g_inference_count_tautology <<
			std::endl;
	}
	std::cout << "Total inferences: " << (g_inference_count_ires1 +
		g_inference_count_gres1 + g_inference_count_cres1 +
		g_inference_count_cres2 + g_inference_count_cres3 +
		g_inference_count_cres4 + g_inference_count_cres5 +
		g_inference_count_tautology) << std::endl;
	std::cout << "Forward subsumed clauses: " << g_forward_subsumed_count <<
		std::endl;
	if(g_backward_subsumption)
	{
		std::cout << "Backward subsumed clauses: " << g_backward_subsumed_count <<
			std::endl;
	}
	std::cout << "Rewritten clauses: " << g_rewrite_count << std::endl;
	if(g_purity_deletion)
	{
		std::cout << "Clauses eliminated in purity deletion: " <<
			g_purity_deletion_clauses_eliminated << std::endl;
	}
	if(g_unit_propagation)
	{
		std::cout << "Clauses eliminated in unit propagation: " <<
			g_unit_propagation_clauses_removed << std::endl;
		std::cout << "Literals eliminated in unit propagation: " <<
			g_unit_propagation_literals_removed << std::endl;
	}
	cleanup();

	// stop the timer
	clock_t end = clock();

	// and display the execution time
	std::cout << "execution finished in " <<
		((double)(end - begin) / CLOCKS_PER_SEC) << " seconds\n";

	return 0;
}

/*--------------------------------------------------------------------------
	resolution

	Exhaustively applies the inference rules to the given list of clauses
	until satisfiability is determined.
--------------------------------------------------------------------------*/
void resolution(std::list<Clause*>* parsed_clauses)
{
	/* Define the first level of the indexes to have a size equal to the
	   number of clause types.

	   The second level has a size equal to |parsed literals| + 1, allowing for
	   the storage of contradictions which we consider to have a maximal literal
	   of rank 0.

	   The final level has a size equal to |parsed literals| + 1, as each list
	   of literals can contain at most |parsed literals| literals without being
	   tautological. In addition a contradiction has 0 literals. */
	ClauseIndex temp_index(NUM_FEATURES, NUM_CLAUSE_TYPES,
		g_literals->size() + 1, g_literals->size() + 1);
	ClauseIndex saturated_index(NUM_FEATURES, NUM_CLAUSE_TYPES,
		g_literals->size() + 1, g_literals->size() + 1);
	ClauseIndex non_saturated_index(NUM_FEATURES, NUM_CLAUSE_TYPES,
		g_literals->size() + 1, g_literals->size() + 1);

	if(g_verbosity >= V_MAXIMAL)
	{
		displayTitle("Initial Self Subsumption");
	}
	// initial self-subsumption
	parsed_clauses = clause_list::initialSelfSubsumption(parsed_clauses, temp_index);
	if(g_purity_deletion)
	{
		// purity deletion
		if(g_verbosity >= V_MAXIMAL)
		{
			displayTitle("Purity Deletion");
		}
		purityDeletion(parsed_clauses);
	}
	if(g_unit_propagation)
	{
		// unit propagation
		if(g_verbosity >= V_MAXIMAL)
		{
			displayTitle("Unit Propagation");
		}
		unitPropagation(parsed_clauses);
	}
	if(g_contradiction != nullptr)
	{
		/* a contradiction was derived from unit propagation so add this to the
		   saturated index and delete the other parsed clauses */
		saturated_index.addToIndex(g_contradiction);
		std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
			[&](Clause* parsed_clause)
			{
				delete parsed_clause;
			});
	}
	else
	{
		// add all the parsed clauses to the non-saturated index
		std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
			[&](Clause* c)
			{
				non_saturated_index.addToIndex(c);
			});

		if(g_verbosity >= V_MAXIMAL)
		{
			displayTitle("Resolution");
		}

		Clause* given;
		if(g_heuristic == GET_NEXT)
		{
			// comparator to compare clauses
			auto comparator =
				[](const Clause* this_clause, const Clause* that_clause)
				{
					return this_clause->getIdentifier()
						< that_clause->getIdentifier();
				};
			// sets for clause selection
			std::set<Clause*, std::function<bool (Clause*, Clause*)>>
				initial_set(comparator);
			std::set<Clause*, std::function<bool (Clause*, Clause*)>>
				universal_set(comparator);
			std::set<Clause*, std::function<bool (Clause*, Clause*)>>
				coalition_set(comparator);
			// add all the clauses to the corresponding set
			std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
				[&](Clause* c)
				{
					if(g_heuristic == GET_NEXT)
					{
						if(c->getClauseType() == ClauseType::UNIVERSAL)
						{
							universal_set.insert(c);
						}
						else if(c->getClauseType() == ClauseType::INITIAL)
						{
							initial_set.insert(c);
						}
						else
						{
							coalition_set.insert(c);
						}
					}
				});
			while(non_saturated_index.getNumClauses() > 0)
			{
				if(!universal_set.empty())
				{
					auto it = universal_set.begin();
					given = *it;
					universal_set.erase(it);
				}
				else if(!coalition_set.empty())
				{
					auto it = coalition_set.begin();
					given = *it;
					coalition_set.erase(it);
				}
				else
				{
					auto it = initial_set.begin();
					given = *it;
					initial_set.erase(it);
				}
				if(g_verbosity >= V_MAXIMAL)
				{
					std::cout << DIVIDER_1;
					std::cout << "Resolving clause:\n";
					given->displayClause();
				}
				// move the clause from non saturated to saturated
				non_saturated_index.removeFromIndex(given);
				saturated_index.addToIndex(given);
				// resolve the clause with the saturated index
				std::list<Clause*>* resolved_list =
					saturated_index.resolve(given);
				// self subsumption
				resolved_list = clause_list::selfSubsumption(resolved_list, temp_index);
				if(g_forward_subsumption)
				{
					// forward subsumption
					resolved_list = clause_list::subsumption(clause_list::subsumption(
								resolved_list, saturated_index),
								non_saturated_index);
				}
				if(g_backward_subsumption)
				{
					// backward subsumption
					clause_list::backwardSubsumptionGetNext(resolved_list,
						saturated_index, &initial_set, &universal_set, &coalition_set);
					clause_list::backwardSubsumptionGetNext(resolved_list,
						non_saturated_index, &initial_set, &universal_set, &coalition_set);
				}
				if(g_verbosity >= V_MAXIMAL)
				{
					// display the simplified non-tautological resolvents
					std::cout << "Resolvents:\n";
					if(resolved_list->size() == 0)
					{
						std::cout << "none\n";
					}
					std::for_each(resolved_list->begin(), resolved_list->end(),
						[&](Clause* c)
						{
							c->displayClause();
						});
					std::cout << DIVIDER_1;
				}
				// add the resolvents to the non saturated index
				std::for_each(resolved_list->begin(), resolved_list->end(),
					[&](Clause* c)
					{
						non_saturated_index.addToIndex(c);
						if(c->getClauseType() == ClauseType::UNIVERSAL)
						{
							universal_set.insert(c);
						}
						else if(c->getClauseType() == ClauseType::INITIAL)
						{
							initial_set.insert(c);
						}
						else
						{
							coalition_set.insert(c);
						}
					});
				if(g_contradiction != nullptr)
				{
					// we have derived a contradiction so we can stop
					saturated_index.addToIndex(g_contradiction);
					delete resolved_list;
					break;
				}
				delete resolved_list;
			}
		}
		else if(g_heuristic == GET_NEXT_SMALLEST)
		{
			while((given = non_saturated_index.getNextSmallestClause()) != nullptr)
			{
				if(g_verbosity >= V_MAXIMAL)
				{
					std::cout << DIVIDER_1;
					std::cout << "Resolving clause:\n";
					given->displayClause();
				}
				// move the clause from non saturated to saturated
				non_saturated_index.removeFromIndex(given);
				saturated_index.addToIndex(given);
				// resolve the clause with the saturated index
				std::list<Clause*>* resolved_list =
					saturated_index.resolve(given);
				// self subsumption
				resolved_list = clause_list::selfSubsumption(resolved_list, temp_index);
				if(g_forward_subsumption)
				{
					// forward subsumption
					resolved_list = clause_list::subsumption(clause_list::subsumption(
								resolved_list, saturated_index),
								non_saturated_index);
				}
				if(g_backward_subsumption)
				{
					// backward subsumption
					clause_list::backwardSubsumption(resolved_list, saturated_index);
					clause_list::backwardSubsumption(resolved_list, non_saturated_index);
				}
				if(g_verbosity >= V_MAXIMAL)
				{
					// display the simplified non-tautological resolvents
					std::cout << "Resolvents:\n";
					if(resolved_list->size() == 0)
					{
						std::cout << "none\n";
					}
					std::for_each(resolved_list->begin(), resolved_list->end(),
						[&](Clause* c)
						{
							c->displayClause();
						});
					std::cout << DIVIDER_1;
				}
				// add the resolvents to the non saturated index
				std::for_each(resolved_list->begin(), resolved_list->end(),
					[&](Clause* c)
					{
						non_saturated_index.addToIndex(c);
					});
				if(g_contradiction != nullptr)
				{
					// we have derived a contradiction so we can stop
					saturated_index.addToIndex(g_contradiction);
					delete resolved_list;
					break;
				}
				delete resolved_list;
			}
		}
	}

	if(g_verbosity >= V_DEFAULT)
	{
		// display the archived and saturated clauses
		displayTitle("Archived Clauses");
		displayArchive();

		displayTitle("Saturated Clauses");
		saturated_index.displayIndex();
	}

	displayTitle("Result");
	if(g_contradiction != nullptr)
	{
		// display any derived contradiction
		if(g_verbosity >= V_DEFAULT)
		{
			std::cout << "Contradiction derived:\n";
			g_contradiction->displayClause();
		}
		std::cout << "Unsatisfiable\n";
	}
	else
	{
		std::cout << "Satisfiable\n";
	}

	delete parsed_clauses;
}

/*--------------------------------------------------------------------------
	unitPropagation

	Exhaustively applies unit propagation the given list of clauses until no
	new unit clauses have been derived or propagated. A contradiction may be
	derived by this procedure.

	For each unit clause with literal p, all non-unit clauses containing p
	are removed from the list, and ~p is removed from all non-unit clauses.
--------------------------------------------------------------------------*/
void unitPropagation(std::list<Clause*>* clauses)
{
	auto unit_clauses = new std::list<Clause*>();
	auto propagated_unit_clauses = new std::list<Clause*>();
	auto clause_it = clauses->begin();
	auto clause_end = clauses->end();
	while(clause_it != clause_end)
	{
		Clause* clause = *clause_it;
		if((clause->getClauseType() == ClauseType::INITIAL ||
			clause->getClauseType() == ClauseType::UNIVERSAL) &&
				clause->size() == 1)
		{
			// we have identified a unit clause
			unit_clauses->push_back(clause);
			clause_it = clauses->erase(clause_it);
		}
		else
		{
			clause_it++;
		}
	}

	while(!unit_clauses->empty())
	{
		Clause* next_unit_clause = unit_clauses->front();
		unit_clauses->pop_front();
		propagated_unit_clauses->push_back(next_unit_clause);
		// propagate this unit clause
		std::list<Clause*>* new_unit_clauses =
			propagateUnitClause(clauses, next_unit_clause);
		// add any newly derived unit clauses to the list
		unit_clauses->splice(unit_clauses->end(), *new_unit_clauses);
		delete new_unit_clauses;

		if(g_contradiction)
		{
			clauses->splice(clauses->begin(), *propagated_unit_clauses);
			clauses->splice(clauses->begin(), *unit_clauses);
			delete propagated_unit_clauses;
			delete unit_clauses;
			return;
		}
	}

	// add the unit clauses back to the original list
	clauses->splice(clauses->begin(), *propagated_unit_clauses);
	delete propagated_unit_clauses;
	delete unit_clauses;
}

/*--------------------------------------------------------------------------
	propagateClause

	Propagates the given unit clause through the list of clauses, and
	returns a list of any newly derived unit clauses.
--------------------------------------------------------------------------*/
std::list<Clause*>* propagateUnitClause(std::list<Clause*>* non_unit_clauses,
	Clause* unit_clause)
{
	auto new_unit_clauses = new std::list<Clause*>();
	std::list<std::list<Clause*>::iterator> remove_list;

	// used to index into the one-dimensional clause literal index
	int identifier_multiplier = Clause::getNextIdentifier();
	// indexes for constant time access to literals in literal lists
	auto clause_literal_index_left = new ListLiteralIteratorPair
		[identifier_multiplier * (g_literals->size() + 1)];
	auto clause_literal_index_right = new ListLiteralIteratorPair
		[identifier_multiplier * (g_literals->size() + 1)];
	// an index to access all clauses containing a given literal
	std::list<std::pair<Clause*, bool>>
		literal_clause_index[g_literals->size() + 1];
	// an index for constant time access to non unit clauses
	std::list<Clause*>::iterator
		non_unit_clause_iterators[Clause::getNextIdentifier()];
	Literal* unit_clause_literal = unit_clause->getRight()->getMaximalLiteral();
	bool unit_initial_clause =
		unit_clause->getClauseType() == ClauseType::INITIAL;

	if(g_verbosity >= V_MAXIMAL)
	{
		std::cout << DIVIDER_1;
		std::cout << "Propagating Unit clause:\n";
		unit_clause->displayClause();
		std::cout << DIVIDER_1;
	}

	auto clauses_it = non_unit_clauses->begin();
	auto clauses_end = non_unit_clauses->end();
	while(clauses_it != clauses_end)
	{
		Clause* non_unit_clause = *clauses_it;
		/* Initial unit clauses can be propagated through other initial clauses
		   and universal clauses can be propagated through clauses of any
		   type. */
		bool include = !unit_initial_clause ||
			(non_unit_clause->getClauseType() == ClauseType::INITIAL);
		if(include)
		{
			int non_unit_clause_id = non_unit_clause->getIdentifier();
			non_unit_clause_iterators[non_unit_clause_id] = clauses_it;
			// set this clause to be active
			non_unit_clause->setActive();
			std::list<Literal*>* left = non_unit_clause->getLeft()->getList();
			std::list<Literal*>* right = non_unit_clause->getRight()->getList();
			if(left != nullptr)
			{
				auto left_it = left->begin();
				auto left_end = left->end();
				while(left_it != left_end)
				{
					Literal* complementary_literal =
						(*left_it)->complementary_literal;
					int complementary_literal_rank =
						complementary_literal->rank;
					std::pair<Clause*, bool> pair;
					pair.first = non_unit_clause;
					pair.second = LEFT;
					/* record that this list contains the negation of this
					   literal */
					literal_clause_index[complementary_literal_rank]
										.push_back(pair);
					int offset = non_unit_clause_id * identifier_multiplier +
						complementary_literal_rank;
					/* and store an iterator to this literal in the list for
					   constant time access */
					clause_literal_index_left[offset].list = left;
					clause_literal_index_left[offset].literal_iterator =
						left_it;
					left_it++;
				}
			}
			if(right != nullptr)
			{
				auto right_it = right->begin();
				auto right_end = right->end();
				while(right_it != right_end)
				{
					Literal* literal = *right_it;
					int literal_rank = literal->rank;
					std::pair<Clause*, bool> pair;
					pair.first = non_unit_clause;
					pair.second = RIGHT;
					/* record that this list contains this literal */
					literal_clause_index[literal_rank].push_back(pair);
					int offset = non_unit_clause_id * identifier_multiplier +
						literal_rank;
					/* and store an iterator to this literal in the list for
					   constant time access */
					clause_literal_index_right[offset].list = right;
					clause_literal_index_right[offset].literal_iterator = right_it;
					right_it++;
				}
			}
		}
		clauses_it++;
	}

	// remove all clauses containing the unit clause literal
	std::for_each(literal_clause_index[unit_clause_literal->rank].begin(),
		literal_clause_index[unit_clause_literal->rank].end(),
		[&](std::pair<Clause*, bool> pair)
		{
			Clause* non_unit_clause = pair.first;
			if(non_unit_clause->isActive())
			{
				g_unit_propagation_clauses_removed++;
				if(g_verbosity >= V_MAXIMAL)
				{
					std::cout << DIVIDER_2;
					std::cout << "Clause eliminated:\n";
					non_unit_clause->displayClause();
					std::cout << DIVIDER_2;
				}
				// set this clause to be inactive
				non_unit_clause->setInactive();
				remove_list.push_back(
					non_unit_clause_iterators[non_unit_clause->getIdentifier()]);
			}
		});
	auto index_it =
		literal_clause_index[unit_clause_literal->complementary_literal->rank]
							.begin();
	auto index_end =
		literal_clause_index[unit_clause_literal->complementary_literal->rank]
							.end();
	// remove the negation of the literal from clauses
	while(index_it != index_end)
	{
		Clause* non_unit_clause = (*index_it).first;
		int non_unit_clause_id = non_unit_clause->getIdentifier();
		if(non_unit_clause->isActive())
		{
			g_unit_propagation_literals_removed++;
			if(g_verbosity >= V_MAXIMAL)
			{
				std::cout << DIVIDER_2;
				std::cout << "Removing literal:\n";
				unit_clause_literal->complementary_literal->displayLiteral();
				std::cout << "From clause:\n";
				non_unit_clause->displayClause();
				std::cout << DIVIDER_2;
			}
			int offset = non_unit_clause_id * identifier_multiplier +
					unit_clause_literal->complementary_literal->rank;
			if((*index_it).second == LEFT)
			{
				// remove the literal from the conjunction in constant time
				ListLiteralIteratorPair pair =
					clause_literal_index_left[offset];
				pair.list->erase(pair.literal_iterator);
			}
			else
			{
				// remove the literal from the disjunction in constant time
				ListLiteralIteratorPair pair =
					clause_literal_index_right[offset];
				pair.list->erase(pair.literal_iterator);
			}

			if(non_unit_clause->getClauseType() == ClauseType::INITIAL ||
				non_unit_clause->getClauseType() == ClauseType::UNIVERSAL)
			{
				if(non_unit_clause->size() == 1)
				{
					// we have a new unit clause so add it to the list
					non_unit_clauses->erase(
						non_unit_clause_iterators[non_unit_clause_id]);
					new_unit_clauses->push_back(non_unit_clause);
				}
			}
			else
			{
				if(non_unit_clause->getLeft()->size() == 0)
				{
					// there are no literals in the conjunction so set it to null
					non_unit_clause->nullifyLeft();
				}
				if(non_unit_clause->getRight()->size() == 0)
				{
					/* there are no literals in the disjunction so rewrite the
					   clause */
					g_rewrite_count++;
					non_unit_clause->setInactive();
					non_unit_clauses->erase(
						non_unit_clause_iterators[non_unit_clause_id]);
					g_archive->push_back(non_unit_clause);
					LiteralList* right = new LiteralList();
					std::list<Literal*>* left =
						non_unit_clause->getLeft()->getList();
					if(left != nullptr)
					{
						std::for_each(left->begin(), left->end(),
							[&](Literal* literal)
							{
								right->addLiteral(
									literal->complementary_literal);
							});
					}
					Justification* j = new Justification(
						non_unit_clause->getIdentifier(),
						-1, nullptr, non_unit_clause->getClauseType() ==
							ClauseType::POSITIVE ? InferenceRule::RW1 :
								InferenceRule::RW2);
					Clause* new_clause = new Clause(new LiteralList(),
						new std::list<int>(), right,
						ClauseType::UNIVERSAL, j, nullptr);
					if(g_verbosity >= V_MAXIMAL)
					{
						std::cout << DIVIDER_2;
						std::cout << "Rewriting clause:\n";
						non_unit_clause->displayClause();
						std::cout << "To:\n";
						new_clause->displayClause();
						std::cout << DIVIDER_2;
					}
					if(right->size() == 1)
					{
						// we have a new unit clause so add it to the list
						new_unit_clauses->push_back(new_clause);
					}
					else if(right->size() == 0)
					{
						// we have a contradiction
						g_contradiction = new_clause;
					}
					else
					{
						non_unit_clauses->push_back(new_clause);
					}
				}
			}
		}
		index_it++;
	}

	// remove all the clauses containing the propagated literal
	std::for_each(remove_list.begin(), remove_list.end(),
		[&](std::list<Clause*>::iterator it)
		{
			Clause* clause = *it;
			non_unit_clauses->erase(it);
			delete clause;
		});

	delete[] clause_literal_index_left;
	delete[] clause_literal_index_right;
	return new_unit_clauses;
}

/*--------------------------------------------------------------------------
	purityDeletion

	Removes all clauses containing pure literals from the given list.
--------------------------------------------------------------------------*/
void purityDeletion(std::list<Clause*>* clauses)
{
	std::list<Clause*>::iterator clauses_it = clauses->begin();
	std::list<Clause*>::iterator clauses_end = clauses->end();

	while(clauses_it != clauses_end)
	{
		Clause* c = *clauses_it;
		Literal* literal_no_benefit = nullptr;
		bool remove_this = false;
		std::list<Literal*>* left_list = c->getLeft()->getList();
		if(left_list != nullptr)
		{
			/* check to see if the complement of (the negation of) any literal
			   in the conjunction is unused */
			std::for_each(left_list->begin(), left_list->end(),
				[&](Literal* l)
				{
					if(!l->is_used)
					{
						literal_no_benefit = l;
						remove_this = true;
					}
				});
		}
		std::list<Literal*>* right_list = c->getRight()->getList();
		if(!remove_this && right_list != nullptr)
		{
			/* check to see if the complement of any literal in the disjunction
			   is unused */
			std::for_each(right_list->begin(), right_list->end(),
				[&](Literal* l)
				{
					if(!l->complementary_literal->is_used)
					{
						literal_no_benefit = l->complementary_literal;
						remove_this = true;
					}
				});
		}
		// this clause can be removed
		if(remove_this)
		{
			g_purity_deletion_clauses_eliminated++;
			if(g_verbosity >= V_MAXIMAL)
			{
				std::cout << DIVIDER_1;
				std::cout << "Deleting clause:\n";
				c->displayClause();
				std::cout << "No occurrences in parsed clauses of:\n";
				literal_no_benefit->displayLiteral();
				std::cout << DIVIDER_1;
			}
			clauses->erase(clauses_it++);
			delete c;
		}
		else
		{
			clauses_it++;
		}
	}
}

/*--------------------------------------------------------------------------
	cleanup

	Free up memory allocated for archived clauses and parsed literals.
--------------------------------------------------------------------------*/
void cleanup()
{
	// delete the clauses in the archive
	std::for_each(g_archive->begin(), g_archive->end(),
		[](Clause* c)
		{
			delete c;
		});

	// delete the parsed literals
	std::for_each(g_literals->begin(), g_literals->end(),
		[](Literal* l)
		{
			delete l;
		});

	delete g_archive;
	delete g_literals;
}

/*--------------------------------------------------------------------------
	displayArchive

	Display all clauses in an archive, sorted into ascending identifier
	order.
--------------------------------------------------------------------------*/
void displayArchive()
{
	// sort the clauses in the archive...
	g_archive->sort(
		[](Clause* this_clause, Clause* that_clause)
		{
			return this_clause->getIdentifier() < that_clause->getIdentifier();
		});
	// then display them
	std::for_each(g_archive->begin(), g_archive->end(),
		[](Clause* clause)
		{
			clause->displayClause();
		});
}

/*--------------------------------------------------------------------------
	displayTitle

	Displays the given string in between two dividers.
--------------------------------------------------------------------------*/
void displayTitle(const std::string& title)
{
	std::cout << DIVIDER_0;
	std::cout << "       " << title << std::endl;
	std::cout << DIVIDER_0;
}
