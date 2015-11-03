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

class Individual {
public:
	Individual();
	Individual(size_t var_count, double const_min, double const_max);
	Individual(size_t var_count, double const_min, double const_max,
		size_t depth_max, bool full_tree);
	void Erase();
	std::string ToString();
	
	/* Genetic Program Functions */
	void GenerateTree(size_t depth_max, bool full_tree);
	void Mutate(double mutation_rate);
	std::pair<Node*, size_t> GetRandomNode(bool nonterminal);
	
	/* Public Helper Functions */
	void CalculateTreeSize();
	void CalculateFitness(std::vector<SolutionData> input_values);

	/* Private Accessors/Mutators */
	double GetFitness();
	size_t GetTreeSize();
	size_t GetTerminalCount();
	size_t GetNonTerminalCount();
	void SetRootNode(Node *root);
private:
	Node *root_;
	double fitness_;
	size_t terminal_count_;
	size_t nonterminal_count_;
	size_t depth_max_;

	/* Helper Functions */
	std::pair<Node*, size_t> SelectNode(size_t countdown, bool nonterminal);
};
