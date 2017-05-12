/*==============================================================================
	File			: parsing.cpp
	Author			: Paul Gainer
	Created			: 22/06/2014
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
#include <fstream>
#include <iostream>

#include "clprover/define.h"
#include "clprover/global.h"
#include "clprover/parsing.h"

#include "file_parser/file_parser.h"
#include "file_parser/file_parser_state.h"

bool parseOrderingFile(const std::string& input_file_name)
{
	std::string literal_name;
	int propositional_variable_count = 0;

	FileParser parser(EXEC_NAME, g_verbosity == V_DEBUG,
		FileParser::IGNORE_WHITESPACE);
	if(!parser.setInputFile(input_file_name))
	{
		return false;
	}

	FileParserState state_0("state 0",
		[&]()
		{
			if(g_verbosity == V_DEBUG)
			{
				std::cout << "parsing propositional variable " <<
					++propositional_variable_count << "...\n";
			}
		});
	FileParserState state_1("state 1");

	state_0.addTransition(FileParserTransition('a', 'z',
		[&](char input_character)
		{
			literal_name = input_character;
		}, state_1));
	state_0.addTransition(FileParserTransition('A', 'Z',
		[&](char input_character)
		{
			literal_name = input_character;
		}, state_1));

	state_1.addTransition(FileParserTransition('a', 'z',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_1));
	state_1.addTransition(FileParserTransition('A', 'Z',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_1));
	state_1.addTransition(FileParserTransition('0', '9',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_1));
	state_1.addTransition(FileParserTransition('_',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_1));
	state_1.addTransition(FileParserTransition('>',
		[&](char input_character)
		{
			constructLiterals(literal_name);
			literal_name = "";
		}, state_0));
	state_1.addTransition(FileParserTransition(EOF,
		[&](char input_character)
		{
			constructLiterals(literal_name);
		}, FileParserState::TERMINAL_STATE));

	parser.setInitialState(state_0);
	return parser.parse();
}

std::list<Clause*>* parseInputFile(const std::string& input_file_name)
{
	std::list<Clause*>* parsed_clauses = new std::list<Clause*>();
	std::list<int> parsed_agents;
	bool is_disjunction;
	bool is_conjunction;
	bool literal_polarity;
	std::string literal_name;
	std::string agent_number;
	LiteralList* left;
	LiteralList* right;
	std::list<int>* agents;
	ClauseType type;
	Clause* contradiction = nullptr;

	FileParser parser(EXEC_NAME, g_verbosity == V_DEBUG,
		FileParser::IGNORE_WHITESPACE);
	if(!parser.setInputFile(input_file_name))
	{
		delete parsed_clauses;
		return nullptr;
	}

	FileParserState state_0("state 0",
		[&]()
		{
			if(g_verbosity == V_DEBUG)
			{
				std::cout << "parsing clause " << Clause::getNextIdentifier() <<
					"...\n";
			}
			literal_name = "";
			literal_polarity = true;
			type = ClauseType::ALL;
			left = new LiteralList();
			right = new LiteralList();
			agents = new std::list<int>();
			is_disjunction = false;
			is_conjunction = false;
		});
	FileParserState state_1("state 1");
	FileParserState state_2("state 2");
	FileParserState state_3("state 3");
	FileParserState state_4("state 4");
	FileParserState state_5("state 5");
	FileParserState state_6("state 6");
	FileParserState state_7("state 7");
	FileParserState state_8("state 8");
	FileParserState state_9("state 9");

	state_0.addTransition(FileParserTransition('a', 'z',
		[&](char input_character)
		{
			literal_name = input_character;
		}, state_4));
	state_0.addTransition(FileParserTransition('A', 'Z',
		[&](char input_character)
		{
			literal_name = input_character;
		}, state_4));
	state_0.addTransition(FileParserTransition('(',
		[&](char input_character)
		{
			type = ClauseType::INITIAL;
		}, state_1));
	state_0.addTransition(FileParserTransition('~',
		[&](char input_character)
		{
			literal_polarity = false;
		}, state_5));
	state_0.addTransition(FileParserTransition('-',
		[&](char input_character)
		{
			is_disjunction = true;
		}, state_6));
	state_0.addTransition(FileParserTransition(';',
		[&](char input_character)
		{
			type = ClauseType::UNIVERSAL;
			Clause* clause = new Clause(left, agents, right, type, nullptr,
				nullptr);
			checkLiteralUse(clause);
			parsed_clauses->push_back(clause);
			if(clause->size() == 0)
			{
				contradiction = clause;
			}
		}, state_0));
	state_0.addTransition(FileParserTransition('.',
		[&](char input_character)
		{
			type = ClauseType::UNIVERSAL;
			Clause* clause = new Clause(left, agents, right, type, nullptr,
				nullptr);
			checkLiteralUse(clause);
			parsed_clauses->push_back(clause);
			if(clause->size() == 0)
			{
				contradiction = clause;
			}
		}, FileParserState::TERMINAL_STATE));

	state_1.addTransition(FileParserTransition('i', nullptr, state_2));

	state_2.addTransition(FileParserTransition(')', nullptr, state_3));

	state_3.addTransition(FileParserTransition('~',
		[&](char input_character)
		{
			literal_polarity = false;
		}, state_5));
	state_3.addTransition(FileParserTransition('a', 'z',
		[&](char input_character)
		{
			literal_name = input_character;
		}, state_4));
	state_3.addTransition(FileParserTransition('A', 'Z',
		[&](char input_character)
		{
			literal_name = input_character;
		}, state_4));
	std::function<void(char)> state_3_function =
		[&](char input_character)
		{
			if(left->size() == 0 && right->size() == 0)
			{
				contradiction = new Clause(left, agents, right, type, nullptr,
					nullptr);
				parsed_clauses->push_back(contradiction);
			}
			else
			{
				if(is_conjunction && (type == ClauseType::ALL ||
					type == ClauseType::INITIAL))
				{
					parser.haltWithError(
						"initial or universal clauses must be a disjunction of literals");
				}
				if(literal_name != "")
				{
					Literal* l = constructAndReturnLiteral(literal_name,
						literal_polarity);
					if(type == ClauseType::ALL)
					{
						type = ClauseType::UNIVERSAL;
					}
					if(is_conjunction)
					{
						left->addLiteral(l);
					}
					else
					{
						right->addLiteral(l);
					}
				}
				Clause* clause = new Clause(left, agents, right, type, nullptr,
					nullptr);
				checkLiteralUse(clause);
				parsed_clauses->push_back(clause);
			}
		};
	state_3.addTransition(FileParserTransition(';', state_3_function,
		state_0));
	state_3.addTransition(FileParserTransition('.', state_3_function,
		FileParserState::TERMINAL_STATE));
	state_3.addTransition(FileParserTransition('|',
		[&](char input_character)
		{
			if(is_conjunction)
			{
				parser.haltWithError("unexpected | in conjunction");
			}
			Literal* l = constructAndReturnLiteral(	literal_name,
				literal_polarity);
			right->addLiteral(l);
			is_disjunction = true;
			literal_name = "";
			literal_polarity = true;
		}, state_3));
	state_3.addTransition(FileParserTransition('&',
		[&](char input_character)
		{
			if(is_disjunction)
			{
				parser.haltWithError("unexpected & in disjunction");
			}
			Literal* l = constructAndReturnLiteral(literal_name,
				literal_polarity);
			left->addLiteral(l);
			is_conjunction = true;
			literal_name = "";
			literal_polarity = true;
		}, state_3));

	state_4.addTransition(FileParserTransition('a', 'z',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_4));
	state_4.addTransition(FileParserTransition('A', 'Z',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_4));
	state_4.addTransition(FileParserTransition('0', '9',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_4));
	state_4.addTransition(FileParserTransition('_',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_4));
	std::function<void(char)> state_4_function =
		[&](char input_character)
		{
			if(is_conjunction && (type == ClauseType::ALL ||
					type == ClauseType::INITIAL))
			{
				parser.haltWithError(
					"initial or universal clauses must be a disjunction of literals");
			}
			Literal* l = constructAndReturnLiteral(literal_name,
				literal_polarity);
			if(type == ClauseType::ALL)
			{
				type = ClauseType::UNIVERSAL;
			}
			if(is_conjunction)
			{
				left->addLiteral(l);
			}
			else
			{
				right->addLiteral(l);
			}
			Clause* clause = new Clause(left, agents, right, type, nullptr,
				nullptr);
			checkLiteralUse(clause);
			parsed_clauses->push_back(clause);
		};
	state_4.addTransition(FileParserTransition(';', state_4_function,
		state_0));
	state_4.addTransition(FileParserTransition('.', state_4_function,
		FileParserState::TERMINAL_STATE));
	state_4.addTransition(FileParserTransition('|',
		[&](char input_character)
		{
			if(is_conjunction)
			{
				parser.haltWithError("unexpected | in conjunction");
			}
			Literal* l = constructAndReturnLiteral(	literal_name,
				literal_polarity);
			right->addLiteral(l);
			is_disjunction = true;
			literal_name = "";
			literal_polarity = true;
		}, state_3));
	state_4.addTransition(FileParserTransition('&',
		[&](char input_character)
		{
			if(is_disjunction)
			{
				parser.haltWithError("unexpected & in disjunction");
			}
			Literal* l = constructAndReturnLiteral(literal_name,
				literal_polarity);
			left->addLiteral(l);
			is_conjunction = true;
			literal_name = "";
			literal_polarity = true;
		}, state_3));
	state_4.addTransition(FileParserTransition('-',
		[&](char input_character)
		{
			if(type == ClauseType::INITIAL)
			{
				parser.haltWithError(
					"initial clauses cannot contain an implication");
			}
			if(is_disjunction)
			{
				parser.haltWithError("invalid implication");
			}
			Literal* l = constructAndReturnLiteral(literal_name,
				literal_polarity);

			left->addLiteral(l);
			is_disjunction = true;
			is_conjunction = false;
			literal_name = "";
			literal_polarity = true;
		}, state_6));

	state_5.addTransition(FileParserTransition('a', 'z',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_4));
	state_5.addTransition(FileParserTransition('A', 'Z',
		[&](char input_character)
		{
			literal_name += input_character;
		}, state_4));

	state_6.addTransition(FileParserTransition('>', nullptr, state_7));

	state_7.addTransition(FileParserTransition('[',
		[&](char input_character)
		{
			type = ClauseType::POSITIVE;
		}, state_8));
	state_7.addTransition(FileParserTransition('<',
		[&](char input_character)
		{
			type = ClauseType::NEGATIVE;
		}, state_8));

	state_8.addTransition(FileParserTransition('1', '9',
		[&](char input_character)
		{
			agent_number = input_character;
		}, state_9));
	state_8.addTransition(FileParserTransition(']',
		[&](char input_character)
		{
			if(type == ClauseType::NEGATIVE)
			{
				parser.haltWithError("expected >");
			}
			if(agents->size() > 0)
			{
				agents->sort([](const int first, const int second)
					{return first < second;});
			}
		}, state_3));
	state_8.addTransition(FileParserTransition('>',
		[&](char input_character)
		{
			if(type == ClauseType::POSITIVE)
			{
				parser.haltWithError("expected ]");
			}
			if(agents->size() > 0)
			{
				agents->sort([](const int first, const int second)
					{return first < second;});
			}
		}, state_3));
	state_8.addTransition(FileParserTransition(',',
		[&](char input_character)
		{
			agents->push_back(atoi(agent_number.c_str()));
			parsed_agents.push_back(atoi(agent_number.c_str()));
		}, state_8));

	state_9.addTransition(FileParserTransition('0', '9',
		[&](char input_character)
		{
			agent_number += input_character;
		}, state_9));
	state_9.addTransition(FileParserTransition(']',
		[&](char input_character)
		{
			if(type == ClauseType::NEGATIVE)
			{
				parser.haltWithError("expected >");
			}
			agents->push_back(atoi(agent_number.c_str()));
			parsed_agents.push_back(atoi(agent_number.c_str()));
			if(agents->size() > 0)
			{
				agents->sort([](const int first, const int second)
					{return first < second;});
			}
		}, state_3));
	state_9.addTransition(FileParserTransition('>',
		[&](char input_character)
		{
			if(type == ClauseType::POSITIVE)
			{
				parser.haltWithError("expected ]");
			}
			agents->push_back(atoi(agent_number.c_str()));
			parsed_agents.push_back(atoi(agent_number.c_str()));
			if(agents->size() > 0)
			{
				agents->sort([](const int first, const int second)
					{return first < second;});
			}
		}, state_3));
	state_9.addTransition(FileParserTransition(',',
		[&](char input_character)
		{
			agents->push_back(atoi(agent_number.c_str()));
			parsed_agents.push_back(atoi(agent_number.c_str()));
		}, state_8));

	parser.setInitialState(state_0);
	if(!parser.parse())
	{
		// there was a parse error so clean up and return
		delete left;
		delete right;
		delete agents;
		std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
		[](Clause* c)
		{
			delete c;
		});
		delete parsed_clauses;
		parsed_clauses = nullptr;
	}
	else
	{
		if(contradiction != nullptr)
		{
			// a contradiction was parsed so display it, clean up, and return
			std::cout << DIVIDER_0;
			std::cout << "       Result\n";
			std::cout << DIVIDER_0;
			if(g_verbosity >= V_DEFAULT)
			{
				std::cout << "Contradiction parsed:\n";
				contradiction->displayClause();
			}
			std::cout << "Not satisfiable\n";
			std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
				[](Clause* c)
				{
					delete c;
				});
			delete parsed_clauses;
			parsed_clauses = nullptr;
		}
		else
		{
			// sort the list of parsed agents and remove duplicates
			parsed_agents.sort();
			parsed_agents.unique();
			int num_agents = parsed_agents.size();
			if(g_num_agents != AGENT_NUM_NOT_SET && g_num_agents > num_agents)
			{
				int next_agent_num = *(parsed_agents.rbegin());
				for(int i = 0; i < (g_num_agents - num_agents); i++)
				{
					parsed_agents.push_back(++next_agent_num);
				}
				num_agents = g_num_agents;
			}
			std::for_each(parsed_clauses->begin(), parsed_clauses->end(),
				[&](Clause* c)
				{
					ClauseType t = c->getClauseType();
					if(t == ClauseType::POSITIVE)
					{
						// build the coalition vector for the positive clause
						int identifier = c->getIdentifier();
						auto agents_it = c->getAgents()->begin();
						auto agents_end = c->getAgents()->end();
						auto parsed_agents_it = parsed_agents.begin();
						for(int i = 0; i < num_agents; i++)
						{
							if((agents_it != agents_end) &&
								(*agents_it == *parsed_agents_it))
							{
								c->addMoveToVector(identifier);
								agents_it++;
							}
							else
							{
								c->addMoveToVector(0);
							}
							parsed_agents_it++;
						}
					}
					else if(t == ClauseType::NEGATIVE)
					{
						// build the coalition vector for the negative clause
						int identifier = -(c->getIdentifier());
						auto agents_it = c->getAgents()->begin();
						auto agents_end = c->getAgents()->end();
						auto parsed_agents_it = parsed_agents.begin();
						for(int i = 0; i < num_agents; i++)
						{
							if((agents_it != agents_end) &&
								(*agents_it == *parsed_agents_it))
							{
								c->addMoveToVector(0);
								agents_it++;
							}
							else
							{
								c->addMoveToVector(identifier);
							}
							parsed_agents_it++;
						}
					}
				});
		}
	}

	return parsed_clauses;
}

void constructLiterals(const std::string& literal_name)
{
	std::pair<Literal*, Literal*>* literal_pair =
		Literal::constructLiteralPairWithOrdering(literal_name);
	g_literals->push_back(literal_pair->second);
	g_literals->push_back(literal_pair->first);
	delete literal_pair;
}

Literal* constructAndReturnLiteral(const std::string& literal_name,
	const bool polarity)
{
	std::vector<Literal*>::iterator loc = std::find_if(g_literals->begin(),
		g_literals->end(),
		[&](Literal* p)
		{
			return p->identifier == literal_name;
		});
	if(loc != g_literals->end())
	{
		Literal* l = *loc;
		return l->polarity == polarity ? l : l->complementary_literal;
	}
	std::pair<Literal*, Literal*>* literal_pair =
		Literal::constructLiteralPair(literal_name);
	Literal* first = literal_pair->first;
	Literal* second = literal_pair->second;
	g_literals->push_back(first);
	g_literals->push_back(second);
	delete literal_pair;
	return polarity ? first : second;
}

void checkLiteralUse(Clause* clause)
{
	std::list<Literal*>* left_list = clause->getLeft()->getList();
	if(left_list != nullptr)
	{
		std::for_each(left_list->begin(), left_list->end(),
			[&](Literal* l)
			{
				l->complementary_literal->is_used = true;
			});
	}
	std::list<Literal*>* right_list = clause->getRight()->getList();
	if(right_list != nullptr)
	{
		std::for_each(right_list->begin(), right_list->end(),
			[&](Literal* l)
			{
				l->is_used = true;
			});
	}
}
