/*
* population.cpp
* UIdaho CS-572: Evolutionary Computation
* Population class: Store a collection of individuals for processing.
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
#include "population.h"
#include <iostream> /* For debugging/logging only */
#include <random>
#include <sstream>

Population::Population(size_t population_size, double mutation_rate,
					   double nonterminal_crossover_rate, 
					   size_t tournament_size, size_t depth_min, 
					   size_t depth_max, double const_min, double const_max, 
					   size_t var_count, std::vector<SolutionData> solutions) {
	solutions_ = solutions;

	mutation_rate_ = mutation_rate;
	nonterminal_crossover_rate_ = nonterminal_crossover_rate;
	tournament_size_ = tournament_size;

	var_count_ = var_count;
	const_min_ = const_min;
	const_max_ = const_max;

	best_fitness_ = DBL_MAX;
	worst_fitness_ = DBL_MIN;
	avg_fitness_ = 0;

	largest_tree_ = 0;
	smallest_tree_ = SIZE_MAX;
	avg_tree_ = 0;

	/* Generate the population */
	if (depth_min > depth_max) {
		size_t temp = depth_max;
		depth_max = depth_min;
		depth_min = temp;
	}
	if (population_size % 2 != 0) {
		++population_size; /* Force population to be even */
	}
	RampedHalfAndHalf(population_size, depth_min, depth_max);
	CalculateFitness();
}

/* Genetic Program Functions */
void Population::RampedHalfAndHalf(size_t population_size, 
	size_t depth_min, size_t depth_max) {
	unsigned gradations = static_cast<unsigned>(depth_max - depth_min + 1);
	pop_.reserve(population_size);
	bool full_tree;
	for (size_t i = 0; i < population_size; ++i) {	
		if (i % 2) {
			full_tree = true;
		} else {
			full_tree = false;
		}
		pop_.push_back(Individual(var_count_, const_min_, const_max_,
			(depth_min + i % gradations), full_tree));
	}
}
void Population::MutatePopulation() {
	for (auto &p : pop_) {
		p.Mutate(mutation_rate_);
	}
}
void Population::Crossover(Individual *parent1, Individual *parent2) {
	/* Get crossover points.  Crossover point of parent1 is the parent
	 * and identifier of whether left or right child.  Crossover point of 
	 * parent2 is the node to splice in.
	 */
	
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> d{ 0,1 };

	bool p1_nonterminal = (d(mt) < nonterminal_crossover_rate_);
	bool p2_nonterminal = (d(mt) < nonterminal_crossover_rate_);
	
	std::pair<Node*, bool> c1 = parent1->GetRandomNode(p1_nonterminal);
	std::pair<Node*, bool> c2 = parent2->GetRandomNode(p2_nonterminal);

	/* c1.first could be nullptr in which case new individual is c2 */
	if (!c1.first->GetParent()) {
		c2.first->SetParent(nullptr);
		parent1->Erase();
		parent1->SetRootNode(c2.first);
	} else {
		Node *parent = c1.first->GetParent();
		c2.first->SetParent(parent);
		if (c1.second) {
			parent->GetLeftChild()->Erase();
			parent->SetLeftChild(c2.first);
		} else {
			parent->GetRightChild()->Erase();
			parent->SetRightChild(c2.first);
		}
	}
	parent1->CorrectTree();
}
void Population::Evolve(size_t elitism_count) {
	std::vector<Individual> evolved_pop(pop_.size());
	std::vector<size_t> elites = Elitism(elitism_count);
	
	/* Choosing elite individuals uses raw fitness */
	for (size_t j = 0; j < elitism_count; ++j) {
		evolved_pop[j] = pop_[elites[j]];
	}

	for (size_t j = elitism_count; j < pop_.size(); ++j) {
		size_t p1 = SelectIndividual();
		size_t p2;
		do {
			p2 = SelectIndividual();
		} while (p2 == p1);

		Individual parent1(pop_[p1]);
		Individual parent2(pop_[p2]);
		Crossover(&parent1, &parent2);

		evolved_pop[j] = parent1;
		evolved_pop[j].Mutate(mutation_rate_);
	}
	this->pop_ = evolved_pop;
	CalculateFitness();
}

/* Helper Functions */
size_t Population::SelectIndividual() {
	size_t winner;
	size_t challenger;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> d{ 0,pop_.size() - 1 };

	winner = d(mt);

	for (size_t i = 0; i < tournament_size_; ++i) {
		challenger = d(mt);
		if (pop_[challenger].GetWeightedFitness() < 
			pop_[winner].GetWeightedFitness()) {
			winner = challenger;
		}
	}
	return winner;
}
std::vector<size_t> Population::Elitism(size_t elitism_count) {
	/*
	* This functions returns a vector of the indices of the
	* "elite_count" most fit individuals in the population.
	* Currently, the elitism is forced at two individuals until I complete
	* a sorting algorithm.
	*/

	size_t best, second;
	best = 0;
	second = 0;

	if (elitism_count % 2 != 0) {
		++elitism_count; /* Force elites to be even */
	}

	for (size_t i = 1; i < pop_.size(); ++i) {
		if (pop_[i].GetFitness() < pop_[best].GetFitness()) {
			best = i;
		} else if (pop_[i].GetFitness() < pop_[second].GetFitness()) {
			second = i;
		}
	}
	return std::vector<size_t>{best, second};
}
void Population::CalculateFitness() {
	/*
	 * Calculating the weighted fitness first calculates the raw fitness 
	 * in order to determine the parsimony coefficient that it needs. 
	 */
	CalculateWeightedFitness();
}
void Population::CalculateRawFitness() {
	double cur_fitness = 0;
	avg_fitness_ = 0;
	best_fitness_ = DBL_MAX;
	worst_fitness_ = DBL_MIN;

	for (size_t i = 0; i < pop_.size(); ++i) {
		pop_[i].CalculateFitness(solutions_);
		cur_fitness = pop_[i].GetFitness();
		avg_fitness_ += cur_fitness;
		if (cur_fitness < best_fitness_) {
			best_fitness_ = cur_fitness;
			best_index_ = i;
		} else if (cur_fitness > worst_fitness_) {
			worst_fitness_ = cur_fitness;
		}
	}
	avg_fitness_ = avg_fitness_ / pop_.size();
}
void Population::CalculateWeightedFitness() {
	/*
	* This calculates both the raw fitness score and the weighted fitness
	* score.  In order to calculate the weighted fitness score, the
	* individual needs to know the constant for parsimony pressure.  In order
	* to derive this constant, the covariance of the solution size and
	* fitness of the population is needed in addition to the variance of
	* the size of the solutions of the population.
	*/
	double parsimony_coefficient = CalculateParsimonyCoefficient();
	double cur_weighted_fitness = 0;
	avg_weighted_fitness_ = 0;
	best_weighted_fitness_ = DBL_MAX;
	worst_weighted_fitness_ = DBL_MIN;

	for (size_t i = 0; i < pop_.size(); ++i) {

		/* Fake parsimony pressure for now. */
		if (pop_[i].GetTreeSize() > 100) {
			parsimony_coefficient = 2;
		} else {
			parsimony_coefficient = 0;
		}

		pop_[i].CalculateWeightedFitness(parsimony_coefficient);
		cur_weighted_fitness = pop_[i].GetWeightedFitness();
		avg_weighted_fitness_ += cur_weighted_fitness;
		if (cur_weighted_fitness < best_weighted_fitness_) {
			best_weighted_fitness_ = cur_weighted_fitness;
			best_weighted_index_ = i;
		} else if (cur_weighted_fitness > worst_weighted_fitness_) {
			worst_weighted_fitness_ = cur_weighted_fitness;
		}
	}
	avg_weighted_fitness_ = avg_weighted_fitness_ / pop_.size();
}
double Population::CalculateParsimonyCoefficient() {
	double covariance = 0;
	double variance = 0;

	CalculateRawFitness();
	CalculateTreeSize();
	
	/*
	for (size_t i = 0; i < pop_.size(); ++i) {
		double cov = static_cast<double>((pop_[i].GetTreeSize() - avg_tree_));
		double var = cov * cov;
		cov *= (pop_[i].GetFitness() - avg_fitness_);
		covariance += (cov / pop_.size());
		variance += (var / pop_.size());
	}

	return covariance / variance;
	*/

	return 1.0f;
}
void Population::CalculateTreeSize() {
	size_t cur_tree = 0;
	avg_tree_ = 0;
	for (auto p : pop_) {
		cur_tree = p.GetTreeSize();
		avg_tree_ += cur_tree;
		if (cur_tree > largest_tree_) {
			largest_tree_ = cur_tree;
		} else if (cur_tree < smallest_tree_) {
			smallest_tree_ = cur_tree;
		}
	}
	total_nodes_ = avg_tree_;
	avg_tree_ = avg_tree_ / pop_.size(); /* Will truncate, I don't care */
}
std::string Population::ToString(bool include_fitness) {
	std::stringstream ss;
	for (auto &p : pop_) {
		if (include_fitness) {
			ss << p.GetFitness() << " ==> ";
		}
		ss << p.ToString() << "\n";
	}
	return ss.str();
}
std::string Population::GetBestSolutionToString(bool include_fitness) {
	std::stringstream ss;

	if (include_fitness) {
		ss << pop_[best_index_].GetFitness() << " ==> ";
	}
	ss << pop_[best_index_].ToString();
	return ss.str();
}
std::string Population::GetBestWeightedSolutionToString(bool include_fitness) {
	std::stringstream ss;

	if (include_fitness) {
		ss << pop_[best_weighted_index_].GetWeightedFitness() << " ==> ";
	}
	ss << pop_[best_weighted_index_].ToString();
	return ss.str();
}

/* Private Accessor Functions */
size_t Population::GetLargestTreeSize() {
	return largest_tree_;
}
size_t Population::GetSmallestTreeSize() {
	return smallest_tree_;
}
size_t Population::GetAverageTreeSize() {
	return avg_tree_;
}
size_t Population::GetTotalNodeCount() {
	return total_nodes_;
}
double Population::GetBestFitness() {
	return best_fitness_;
}
double Population::GetWorstFitness() {
	return worst_fitness_;
}
double Population::GetAverageFitness() {
	return avg_fitness_;
}
double Population::GetBestWeightedFitness() {
	return best_weighted_fitness_;
}
double Population::GetWorstWeightedFitness() {
	return worst_weighted_fitness_;
}
double Population::GetAverageWeightedFitness() {
	return avg_weighted_fitness_;
}