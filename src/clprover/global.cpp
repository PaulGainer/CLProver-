/*==============================================================================
	File			: global.cpp
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
#include "clprover/global.h"
#include "clprover/define.h"

const std::string EXEC_VERSION = "1.0.3";
const std::string EXEC_NAME = "CLProver++";

std::list<Clause*>* g_archive = new std::list<Clause*>();
std::vector<Literal*>* g_literals = new std::vector<Literal*>();
Clause* g_contradiction = nullptr;
int g_heuristic = GET_NEXT_SMALLEST;
int g_num_agents = AGENT_NUM_NOT_SET;
int g_verbosity = V_DEFAULT;
int g_inference_count_tautology = 0;
int g_inference_count_ires1 = 0;
int g_inference_count_gres1 = 0;
int g_inference_count_cres1 = 0;
int g_inference_count_cres2 = 0;
int g_inference_count_cres3 = 0;
int g_inference_count_cres4 = 0;
int g_inference_count_cres5 = 0;
int g_backward_subsumed_count = 0;
int g_forward_subsumed_count = 0;
int g_rewrite_count = 0;
int g_unit_propagation_clauses_removed = 0;
int g_unit_propagation_literals_removed = 0;
int g_purity_deletion_clauses_eliminated = 0;
bool g_increment_literal_ranks = true;
bool g_display_modality_as_vector = false;
bool g_display_extra_inference_information = false;
bool g_unit_propagation = false;
bool g_purity_deletion = false;
bool g_forward_subsumption = true;
bool g_backward_subsumption = false;
