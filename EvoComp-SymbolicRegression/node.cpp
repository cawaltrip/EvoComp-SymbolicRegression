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
#include <deque>
#include <iostream> /* Only for errors */
#include <random>
#include <string> /* For std::to_string */
#include "operator_types.h"

void Node::Copy(Node *to_copy) {
	parent_ = to_copy->parent_;
	op_ = to_copy->op_;
	var_count_ = to_copy->var_count_;
	const_min_ = to_copy->const_min_;
	const_max_ = to_copy->const_max_;

	switch (op_) {
	case kConst:
		const_val_ = to_copy->const_val_;
		left_ = nullptr;
		right_ = nullptr;
		break;
	case kVar:
		left_ = nullptr;
		right_ = nullptr;
		var_index_ = to_copy->var_index_;
		break;
	default:
		/* Leaf node, so copy children */
		left_ = new Node;
		right_ = new Node;
		left_->Copy(to_copy->left_);
		right_->Copy(to_copy->right_);
	}
}
void Node::Erase() {
	if (left_) {
		left_->Erase();
	}
	if (right_) {
		right_->Erase();
	}
	delete this;
}
std::string Node::ToString() {
	switch (op_) {
	case kAdd:
		return ("(" + left_->ToString() +
				" + " + right_->ToString() + ")");
		break;
	case kSub:
		return ("(" + left_->ToString() +
				" - " + right_->ToString() + ")");
		break;
	case kMult:
		return ("(" + left_->ToString() +
				" * " + right_->ToString() + ")");
		break;
	case kDiv:
		return ("(" + left_->ToString() +
				" / " + right_->ToString() + ")");
		break;
	case kConst:
		return std::to_string(const_val_);
		break;
	case kVar:
		return "X_" + std::to_string(var_index_);
		break;
	default:
		return "ERROR REACHED";
		break;
	}
}
void Node::GenerateTree(size_t cur_depth, size_t max_depth,
						Node *parent, bool full_tree) {
	std::random_device rd;
	std::mt19937 mt(rd());

	OpType lower_bound, upper_bound;
	parent_ = parent;

	if (full_tree) {
		lower_bound = kAdd;
		upper_bound = kDiv;
	} else {
		lower_bound = kAdd;
		upper_bound = kVar;
	}
	if (cur_depth >= max_depth) {
		lower_bound = kConst;
		upper_bound = kVar;
	}

	std::uniform_int_distribution<int> d{ lower_bound, upper_bound };
	op_ = static_cast<OpType>(d(mt));
	switch (op_) {
	case kAdd:
	case kSub:
	case kMult:
	case kDiv:
		for (size_t i = 0; i < 2; ++i) {
			Node *child = new Node;
			if (!i) {
				left_ = child;
			} else {
				right_ = child;
			}
			child->var_count_ = this->var_count_;
			child->const_min_ = this->const_min_;
			child->const_max_ = this->const_max_;
			child->GenerateTree(cur_depth + 1, max_depth, this, full_tree);
		}
		break;
	case kConst:
		const_val_ = GenerateConstantValue();
		left_ = nullptr;
		right_ = nullptr;
		break;
	case kVar:
		var_index_ = GenerateVariableIndex();
		left_ = nullptr;
		right_ = nullptr;
		break;
	default:
		/* Shouldn't get here */
		std::cerr << "Bad node type!" << std::endl;
		exit(EXIT_FAILURE);
		break;
	}
}
void Node::Mutate(double mutation_chance) {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> mut_dist{ 0,1 };
	
	
	if (mut_dist(mt) <= mutation_chance) {
		OpType lower_bound, upper_bound;
		if (IsTerminal()) {
			lower_bound = kConst;
			upper_bound = kVar;
		} else {
			lower_bound = kAdd;
			upper_bound = kDiv;
		}
		std::uniform_int_distribution<int> d{ lower_bound, upper_bound };
		OpType temp = op_; /* Only change if other terminal type chosen */
		op_ = static_cast<OpType>(d(mt));
		switch (op_) {
		case kConst:
			if (temp == kVar) {
				const_val_ = GenerateConstantValue();
			}
			break;
		case kVar:
			if (temp == kConst) {
				var_index_ = GenerateVariableIndex();
			}
		}
	}
	if (IsNonTerminal()) {
		left_->Mutate(mutation_chance);
		right_->Mutate(mutation_chance);
	}
}
double Node::Evaluate(std::vector<double> var_values) {
	double right_fitness;
	switch (op_) {
	case kAdd:
		return left_->Evaluate(var_values) + right_->Evaluate(var_values);
		break;
	case kSub:
		return left_->Evaluate(var_values) - right_->Evaluate(var_values);
		break;
	case kMult:
		return left_->Evaluate(var_values) * right_->Evaluate(var_values);
		break;
	case kDiv:
		right_fitness = right_->Evaluate(var_values);
		if (right_fitness == 0) {
			right_fitness = 1; /* Safe Division */
		}
		return left_->Evaluate(var_values) / right_fitness;
		break;
	case kConst:
		return const_val_;
		break;
	case kVar:
		return var_values[var_index_]; /* No bounds checking here */
		break;
	default:
		std::cerr << "Node has no OpType!" << std::endl;
		exit(EXIT_FAILURE);
		break;
	}
}
std::pair<Node*, bool> Node::SelectNode(size_t countdown, bool nonterminal) {
	std::deque<std::pair<Node*, bool>> stack;
	std::pair<Node*, bool> current;
	bool done = false;

	stack.push_front(std::make_pair(this, true));

	while (!done) {
		if (stack.empty()) {
			std::cerr << "Empty stack!" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (countdown == SIZE_MAX) {
			std::cerr << "Countdown wrapped around!" << std::endl;
			exit(EXIT_FAILURE);
		}
		current = stack.front();
		stack.pop_front();
		if (countdown == 0 && nonterminal == current.first->IsNonTerminal()) {
			/* Found the node that we want. */
			done = true;
			return current;
			break;
		}
		if (nonterminal == current.first->IsNonTerminal()) {
			--countdown;
		}
		
		if (current.first->right_) {
			stack.push_front(std::make_pair(current.first->right_, false));
		}
		if (current.first->left_) {
			stack.push_front(std::make_pair(current.first->left_, true));
		}
	}
	return current;
}

/* Helper Functions */
void Node::CountNodes(size_t &term_count, size_t &nonterm_count) {
	switch (op_) {
	case kAdd:
	case kSub:
	case kMult:
	case kDiv:
		++nonterm_count;
		left_->CountNodes(term_count, nonterm_count);
		right_->CountNodes(term_count, nonterm_count);
		break;
	case kConst:
	case kVar:
		++term_count;
		break;
	}
}
double Node::GenerateConstantValue() {
	std::random_device rd;
	std::mt19937 mt(rd());

	std::uniform_real_distribution<double> d{ const_min_,const_max_ };
	return d(mt);
}
size_t Node::GenerateVariableIndex() {
	std::random_device rd;
	std::mt19937 mt(rd());

	std::uniform_int_distribution<size_t> d{ 0,var_count_ };
	return d(mt);
}
bool Node::IsNonTerminal() {
	switch (op_) {
	case kAdd:
	case kSub:
	case kMult:
	case kDiv:
		return true;
	}
	return false;
}
bool Node::IsTerminal() {
	return !IsNonTerminal();
}
void Node::CorrectParents(Node *parent) {
	this->parent_ = parent;
	if (IsNonTerminal()) {
		this->left_->CorrectParents(this);
		this->right_->CorrectParents(this);
	}
}

/* Private Accessors/Mutators */
Node* Node::GetParent() {
	return parent_;
}
Node* Node::GetLeftChild() {
	return left_;
}
Node* Node::GetRightChild() {
	return right_;
}
void Node::SetParent(Node *parent) {
	parent_ = parent;
}
void Node::SetLeftChild(Node *child) {
	if (!left_) {
		left_ = new Node;
	}
	left_ = child;
}
void Node::SetRightChild(Node *child) {
	if (!right_) {
		right_ = new Node;
	}
	right_ = child;
}
void Node::SetVarCount(size_t var_count) {
	var_count_ = var_count;
}
void Node::SetConstMin(double const_min) {
	const_min_ = const_min;
}
void Node::SetConstMax(double const_max) {
	const_max_ = const_max;
}
