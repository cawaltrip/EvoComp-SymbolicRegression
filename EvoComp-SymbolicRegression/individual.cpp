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
#include <deque>
#include <random>

Individual::Individual(size_t var_count, double const_min, double const_max) {
	root_ = new Node;
	root_->SetVarCount(var_count);
	root_->SetConstMin(const_min);
	root_->SetConstMax(const_max);
	root_->SetChildNumber(0);
}
Individual::Individual() : Individual(0, 0, 0) {}
Individual::Individual(size_t var_count, double const_min, double const_max,
	size_t depth_max, bool full_tree) 
	: Individual(var_count, const_min, const_max) {
	this->GenerateTree(depth_max, full_tree);
}
void Individual::Erase() {
	root_->Erase();
	delete root_;
}
void Individual::Mutate(double mutation_rate) {
	root_->Mutate(mutation_rate);
}
std::pair<Node*, size_t> Individual::GetRandomNode(bool nonterminal) {
	std::random_device rd;
	std::mt19937 mt(rd());
	size_t upper_bound;
	if (nonterminal && !nonterminal_count_) {
		nonterminal = false;
	}
	if (nonterminal) {
		upper_bound = nonterminal_count_ - 1;
	} else {
		upper_bound = terminal_count_ - 1;
	}
	std::uniform_int_distribution<size_t> d{ 0,upper_bound };
	size_t countdown = d(mt);
	
	/* 
	std::pair<Node*, size_t> node_pair;
	if ((nonterminal && !nonterminal_count_) || (!nonterminal && terminal_count_) ) {
		node_pair = std::make_pair(root_, 0);
	} else {
		node_pair = root_->SelectNode(countdown, nonterminal);
	}
	return node_pair;
	*/
	return root_->SelectNode(countdown, nonterminal);
	
}
void Individual::CalculateTreeSize() {
	terminal_count_ = 0;
	nonterminal_count_ = 0;
	root_->CountAndCorrectNodes(terminal_count_, nonterminal_count_);
}
void Individual::SetRootNode(Node *root) {
	root_ = root;
}
void Individual::CalculateFitness(std::vector<SolutionData> solutions) {
	fitness_ = 0.0f;
	for (size_t i = 0; i < solutions.size(); ++i) {
		fitness_ += pow(solutions[i].y - root_->Evaluate(solutions[i].x),2);
	}
	fitness_ = sqrt(fitness_ / solutions.size());
}
void Individual::GenerateTree(size_t depth_max, bool full_tree) {
	root_->GenerateTree(0, depth_max, nullptr, full_tree);
	CalculateTreeSize();
}
std::string Individual::ToString() {
	return root_->ToString();
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