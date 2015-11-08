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
#include <iostream>
#include <random>
#include <string>

Individual::Individual(size_t var_count, double const_min, double const_max) {
	root_ = new Node;
	root_->SetVarCount(var_count);
	root_->SetConstMin(const_min);
	root_->SetConstMax(const_max);
}
Individual::Individual() : Individual(0, 0, 0) {}
Individual::Individual(size_t var_count, double const_min, double const_max,
	size_t depth_max, bool full_tree) 
	: Individual(var_count, const_min, const_max) {
	this->GenerateTree(depth_max, full_tree);
}
Individual::Individual(const Individual &to_copy) {
	root_ = new Node;
	root_->Copy(to_copy.root_);
	fitness_ = to_copy.fitness_;
	terminal_count_ = to_copy.terminal_count_;
	nonterminal_count_ = to_copy.nonterminal_count_;

	CorrectTree();
}
void Individual::Copy(Individual *to_copy) {
	root_ = to_copy->root_;
	fitness_ = to_copy->fitness_;
	terminal_count_ = to_copy->terminal_count_;
	nonterminal_count_ = to_copy->nonterminal_count_;
}
void Individual::Erase() {
	root_->Erase();
}
std::string Individual::ToString() {
	return root_->ToString();
}

/* Genetic Program Functions */
void Individual::GenerateTree(size_t depth_max, bool full_tree) {
	root_->GenerateTree(0, depth_max, nullptr, full_tree);
	CalculateTreeSize();
}
void Individual::Mutate(double mutation_rate) {
	root_->Mutate(mutation_rate);
}
std::pair<Node*, bool> Individual::GetRandomNode(bool nonterminal) {
	/* Returned bool is true for left child and false for right child. */
	/* Can return nullptr, which means that entire tree will be replaced. */
	std::random_device rd;
	std::mt19937 mt(rd());
	size_t upper_bound;
	size_t countdown;

	/* Even if nonterminal is true, return root node if it's the only node */
	if (nonterminal_count_ == 0) {
		return std::pair<Node*, bool>(root_, nonterminal);
	}

	if (nonterminal) {
		upper_bound = nonterminal_count_ - 1;
	} else {
		upper_bound = terminal_count_ - 1;
	}
	std::uniform_int_distribution<size_t> d{ 0,upper_bound };
	countdown = d(mt);

	return root_->SelectNode(countdown, nonterminal);
}

/* Helper Functions */
void Individual::CalculateTreeSize() {
	terminal_count_ = 0;
	nonterminal_count_ = 0;
	root_->CountNodes(terminal_count_, nonterminal_count_);
}
void Individual::CalculateFitness(std::vector<SolutionData> solutions) {
	fitness_ = 0.0;
	for (size_t i = 0; i < solutions.size(); ++i) {
		fitness_ += pow(solutions[i].y - root_->Evaluate(solutions[i].x), 2);
	}
	fitness_ = sqrt(fitness_ / solutions.size());
}
void Individual::CalculateWeightedFitness(double parsimony_coefficient) {
	weighted_fitness_ = fitness_ + parsimony_coefficient * GetTreeSize();
}
void Individual::CorrectTree() {
	root_->CorrectParents(nullptr);
	CalculateTreeSize();
}


/* Private Accessors/Mutators */
double Individual::GetFitness() {
	return fitness_;
}
double Individual::GetWeightedFitness() {
	return weighted_fitness_;
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
Node* Individual::GetRootNode() {
	return root_;
}
void Individual::SetRootNode(Node *root) {
	root_ = root;
}
