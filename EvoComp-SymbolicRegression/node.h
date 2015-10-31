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
	void GenerateFullTree(size_t cur_depth, size_t max_depth, Node *parent);
	double Evaluate(std::vector<double> var_values);
	void Print();
	void CountNodes(size_t &term, size_t &nonterm);
	void SetParent(Node *parent);
	void SetVarCount(size_t var_count);
	void SetConstMin(double const_min);
	void SetConstMax(double const_max);
private:
	/* Tree Structure */
	Node *parent_;
	std::vector<Node*> children_;

	/* Node Data */
	OpType op_;
	size_t var_index_;
	double const_val_;

	/* Node Metadata */
	size_t var_count_;
	double const_min_;
	double const_max_;
};