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

void Node::Copy(Node *to_copy) {
	terminal_count_ = to_copy->terminal_count_;
	nonterminal_count_ = to_copy->nonterminal_count_;
	depth_limit_ = to_copy->depth_limit_;
	op_ = to_copy->op_;
	var_count_ = to_copy->var_count_;
	const_min_ = to_copy->const_min_;
	const_max_ = to_copy->const_max_;

	if (op_ == kConst) {
		const_val_ = to_copy->const_val_;
	} else if (op_ == kVar) {
		var_index_ = to_copy->var_index_;
	} else {
		/* We must not be a leaf node, so copy children too */
		for (size_t i = 0; i < to_copy->children_.size(); ++i) {
			if (to_copy->children_[i] != nullptr) {
				children_[i] = new Node();
				children_[i]->Copy(to_copy->children_[i]);
				children_[i]->parent_ = this;
			}
		}
	}
}
void Node::Erase() {
	for (auto &child : children_) {
		child->parent_ = nullptr;
		child->Erase();
	}
	delete this;
}
