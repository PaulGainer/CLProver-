/*==============================================================================
	Defines

	File			: define.h
	Author			: Paul Gainer
	Created			: 22/06/2014
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
#ifndef DEFINE_H_
#define DEFINE_H_

#define LEFT true
#define RIGHT false

#define AGENT_NUM_NOT_SET -1

// we use 3 features to characterize a clause
#define NUM_FEATURES 3
// there are 4 different types of clause
#define NUM_CLAUSE_TYPES 4

// verbosity defines
#define V_MINIMAL 0
#define V_DEFAULT 1
#define V_MAXIMAL 2
#define V_DEBUG 3

// heuristic defines
#define GET_NEXT_SMALLEST 0
#define GET_NEXT 1

// dividers
#define DIVIDER_0  "================================================================================\n"
#define DIVIDER_1  "________________________________________________________________________________\n"
#define DIVIDER_2  "--------------------------------------------------------------------------------\n"

#endif
