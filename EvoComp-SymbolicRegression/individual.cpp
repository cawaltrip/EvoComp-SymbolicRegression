/*
* individual.cpp
* UIdaho CS-572: Evolutionary Computation
* Individual class.  Defines a single potential solution to
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
#include "individual.h"
#include <iostream> /* For debug/logging only */


Individual::Individual(size_t max_depth, size_t var_count,
					   double const_min, double const_max) {
	depth_limit_ = max_depth;
	root_ = new Node;
	//root_->SetParent(nullptr);
	root_->SetVarCount(var_count);
	root_->SetConstMin(const_min);
	root_->SetConstMax(const_max);

	GenerateFullTree();
}


Individual::~Individual() {
	root_->Erase();
	delete root_;
}


void Individual::CalculateTreeSize() {
	//std::pair<size_t, size_t> node_counts;
	//node_counts = root_->CountNodeTypes();
	//terminal_count_ = node_counts.first;
	//nonterminal_count_ = node_counts.second;
}

void Individual::CalculateFitness(std::vector<SolutionData> input_values) {
	fitness_ = 0.0f;
	std::vector<double> output_values(input_values[0].x.size());
	for (size_t i = 0; i < input_values.size(); ++i) {
		output_values[i] = root_->Evaluate(input_values[i].x);
	}
	for (auto x : output_values) {
		std::clog << x << std::endl;
	}
}

void Individual::GenerateFullTree() {
	root_->GenerateFullTree(0, depth_limit_, nullptr);
}


double Individual::GetFitness() {
	return fitness_;
}

size_t Individual::GetTreeSize() {
	return terminal_count_ + nonterminal_count_;
}