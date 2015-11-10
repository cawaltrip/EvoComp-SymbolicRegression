/*
 * node.h
 * UIdaho CS-572: Evolutionary Computation
 * Header for Node class - a node of a tree data structure
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

#include <utility> /* std::pair */
#include <vector>
#include "operator_types.h"

class Node {
public:
	void Copy(Node *to_copy);
	void Erase();
	std::string ToString(bool latex = false);

	void GenerateTree(size_t cur_depth, size_t max_depth,
		Node *parent, bool full_tree);
	void Mutate(double mutation_chance);
	double Evaluate(std::vector<double> var_values);
	std::pair<Node*,bool> SelectNode(size_t countdown, bool nonterminal);

	/* Public Helper Functions */
	void CountNodes(size_t &term_count, size_t &nonterm_count);
	bool IsTerminal();
	bool IsNonTerminal();
	void CorrectParents(Node *parent);

	/* Private Accessors/Mutators */
	Node* GetParent();
	Node* GetLeftChild();
	Node* GetRightChild();
	void SetParent(Node *parent);
	void SetLeftChild(Node *child);
	void SetRightChild(Node *child);
	void SetVarCount(size_t var_count);
	void SetConstMin(double const_min);
	void SetConstMax(double const_max);
private:
	/* Private Helper Functions */
	double GenerateConstantValue();
	size_t GenerateVariableIndex();

	/* Tree Structure */
	Node *parent_;
	Node *left_;
	Node *right_;

	/* Node Data */
	OpType op_;
	size_t var_index_;
	double const_val_;

	/* Node Metadata */
	size_t var_count_;
	double const_min_;
	double const_max_;
};