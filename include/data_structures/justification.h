/*==============================================================================
	Justification

	A data structure containing information about the derivation of a clause,
	consisting of the identifiers of the clauses from which the clause was
	resolved (a single identifier if the clause was the result of an application
	of RW1 or RW2), the literal that was resolved, and the inference rule
	applied.

	File			: justification.h
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
#ifndef JUSTIFICATION_H_
#define JUSTIFICATION_H_

#include "data_structures/clause.h"
#include "data_structures/literal.h"

#include "enums/inference_rule.h"

/*==============================================================================
	Justification
==============================================================================*/
struct Justification
{
	/*==========================================================================
		Struct Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The first clause from which this clause was resolved.
	--------------------------------------------------------------------------*/
	const int input_clause_1;

	/*--------------------------------------------------------------------------
		The second clause from which this clause was resolved. Null if this
		clause was a rewritten clause.
	--------------------------------------------------------------------------*/
	const int input_clause_2;

	/*--------------------------------------------------------------------------
		The literal resolved to derive this clause.
	--------------------------------------------------------------------------*/
	const Literal* resolved_literal;

	/*--------------------------------------------------------------------------
		The inference rule applied to derive this clause.
	--------------------------------------------------------------------------*/
	InferenceRule inference_rule;

	/*==========================================================================
		Struct Constructor
	==========================================================================*/
	Justification(const int input_clause_1, const int input_clause_2,
		const Literal* resolved_literal, InferenceRule inference_rule):
			input_clause_1(input_clause_1), input_clause_2(input_clause_2),
			resolved_literal(resolved_literal), inference_rule(inference_rule){}
};

#endif
