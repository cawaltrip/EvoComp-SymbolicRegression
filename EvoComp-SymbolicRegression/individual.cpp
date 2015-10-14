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

Individual::Individual(size_t max_depth, size_t var_count,
					   double const_min, double const_max) {
	max_depth_ = max_depth;
	root_ = new Node(var_count, const_min, const_max, nullptr);
	GenerateFullTree();
}


Individual::~Individual() {
	root_->EraseTrees();
	delete root_;
}


void Individual::CalculateTreeSize() {
	std::pair<size_t, size_t> node_counts;
	node_counts = root_->CountNodeTypes();
	terminal_count_ = node_counts.first;
	nonterminal_count_ = node_counts.second;
}

void Individual::CalculateFitness(std::vector<double> input_values, 
								  std::vector<double> output_values) {
	fitness_ = 0.0f;
}

void Individual::GenerateFullTree() {
	root_->GenerateFullTree(0, max_depth_);
}


double Individual::GetFitness() {
	return fitness_;
}

size_t Individual::GetTreeSize() {
	return terminal_count_ + nonterminal_count_;
}