/*
* individual.h
* UIdaho CS-572: Evolutionary Computation
* Header for Individual class.  Defines a single potential solution to 
* a symbolic regression problem.
*
* Copyright (C) 2015 Chris Waltrip <walt2178@vandals.uidaho.edu>
*
* This file is part of EC-SymbolicReg
*
* EC-SymbolicReg is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* EC-SymbolicReg is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with EC-SymbolicReg.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <utility>
#include <vector>
#include "node.h"

class Individual {
public:
	Individual(size_t max_depth, size_t var_count, 
			   double const_min, double const_max);
	~Individual();

	void CalculateTreeSize();
	void CalculateFitness(std::vector<double> input_values, 
						 std::vector<double> output_values);
	void GenerateFullTree();

	double GetFitness();
	size_t GetTreeSize();
private:
	Node *root_;
	double fitness_;
	size_t terminal_count_;
	size_t nonterminal_count_;
	size_t max_depth_;
};
