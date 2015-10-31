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
#include "solution_data.h"

const size_t kMaxDepth = 6;

class Individual {
public:
	Individual(size_t depth_max, size_t var_count, 
		double const_min, double const_max, bool full_tree);
	void Erase();

	void CalculateTreeSize();
	void CalculateFitness(std::vector<SolutionData> input_values);
	void GenerateTree(bool full_tree);

	double GetFitness();
	size_t GetTreeSize();
	size_t GetTerminalCount();
	size_t GetNonTerminalCount();
	void PrintTree();
private:
	Node *root_;
	double fitness_;
	size_t terminal_count_;
	size_t nonterminal_count_;
	size_t depth_max_;
};
