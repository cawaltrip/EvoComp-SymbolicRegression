/*
* node.cpp
* UIdaho CS-572: Evolutionary Computation
* Node class - a node of a tree data structure
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
#include "node.h"
#include <algorithm>

Node::Node(size_t var_count, double const_min, 
		   double const_max, Node *parent) {
	var_count_ = var_count;
	const_min_ = const_min;
	const_max_ = const_max;
	parent_ = parent;
}

void Node::EraseTrees() {
	for (std::vector<Node*>::iterator it = children_.begin(); 
		it != children_.end(); ++it) {
		if (*it != nullptr) {
			(*it)->EraseTrees();
			delete (*it);
		}
	}
	children_.clear();
	parent_ = nullptr;
}

void Node::GenerateFullTree(size_t depth, size_t max_depth) {

}

void Node::GenerateTree(size_t depth, size_t max_depth) {

}

double Node::EvaluateFitness() {

	return 0.0;
}

std::pair<size_t, size_t> CountNodeTypes() {
	std::pair<size_t, size_t> holder = std::make_pair(-1, -1);
	return holder;
}
