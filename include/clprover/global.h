/*==============================================================================
	Global variable definitions

	File			: global.h
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
#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <functional>
#include <list>
#include <set>
#include <string>
#include <vector>

#include "data_structures/clause.h"

extern const std::string EXEC_VERSION;
extern const std::string EXEC_NAME;

extern std::list<Clause*>* g_archive;
extern std::vector<Literal*>* g_literals;
extern Clause* g_contradiction;
extern int g_heuristic;
extern int g_num_agents;
extern int g_verbosity;
extern int g_inference_count_tautology;
extern int g_inference_count_ires1;
extern int g_inference_count_gres1;
extern int g_inference_count_cres1;
extern int g_inference_count_cres2;
extern int g_inference_count_cres3;
extern int g_inference_count_cres4;
extern int g_inference_count_cres5;
extern int g_backward_subsumed_count;
extern int g_forward_subsumed_count;
extern int g_rewrite_count;
extern int g_unit_propagation_clauses_removed;
extern int g_unit_propagation_literals_removed;
extern int g_purity_deletion_clauses_eliminated;
extern bool g_increment_literal_ranks;
extern bool g_display_extra_inference_information;
extern bool g_display_modality_as_vector;
extern bool g_unit_propagation;
extern bool g_purity_deletion;
extern bool g_forward_subsumption;
extern bool g_backward_subsumption;

#endif
