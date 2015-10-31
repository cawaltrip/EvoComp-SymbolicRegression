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

Individual::Individual(size_t depth_max, size_t var_count,
					   double const_min, double const_max, bool full_tree) {
	depth_max_ = depth_max;
	root_ = new Node;
	root_->SetVarCount(var_count);
	root_->SetConstMin(const_min);
	root_->SetConstMax(const_max);

	if (full_tree) {
		GenerateFullTree();
	} else {
		GenerateSparseTree();
	}
}
void Individual::Erase() {
	root_->Erase();
	delete root_;
}
void Individual::CalculateTreeSize() {
	terminal_count_ = 0;
	nonterminal_count_ = 0;
	root_->CountNodes(terminal_count_, nonterminal_count_);
}
void Individual::CalculateFitness(std::vector<SolutionData> solutions) {
	fitness_ = 0.0f;
	for (size_t i = 0; i < solutions.size(); ++i) {
		fitness_ += pow(solutions[i].y - root_->Evaluate(solutions[i].x),2);
	}
	fitness_ += sqrt(fitness_);
}
void Individual::GenerateFullTree() {
	root_->GenerateFullTree(0, depth_max_, nullptr);
}
void Individual::GenerateSparseTree() {
	root_->GenerateSparseTree(0, depth_max_, nullptr);
}
double Individual::GetFitness() {
	return fitness_;
}
size_t Individual::GetTreeSize() {
	return this->GetTerminalCount() + this->GetNonTerminalCount();
}
size_t Individual::GetTerminalCount() {
	return terminal_count_;
}
size_t Individual::GetNonTerminalCount() {
	return nonterminal_count_;
}