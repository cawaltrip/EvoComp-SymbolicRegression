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
//#include <string>

Population::Population(size_t population_size, double mutation_rate,
	double nonterminal_crossover_rate, size_t tournament_size,
	size_t depth_min, size_t depth_max, double const_min, double const_max,
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
	CalculateTreeSize();
}
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
void Population::Evolve(size_t evolution_count, size_t elitism_count) {
	for (size_t i = 0; i < evolution_count; ++i) {
		std::vector<Individual> evolved_pop(pop_.size());
		std::vector<size_t> elites = Elitism(elitism_count);
		for (size_t j = 0; j < elitism_count; ++j) {
			evolved_pop[j] = pop_[elites[j]];
		}
		/* TODO (Chris): This should only generate one offspring at a time */
		for (size_t j = elitism_count; j < pop_.size(); j += 2) {
			size_t parent1 = SelectIndividual();
			size_t parent2 = SelectIndividual();

			Crossover(&pop_[parent1], &pop_[parent2]);
			pop_[parent1].Mutate(mutation_rate_);
			pop_[parent2].Mutate(mutation_rate_);

			evolved_pop[j] = pop_[parent1];
			evolved_pop[j + 1] = pop_[parent2];
		}
		pop_ = evolved_pop;
		CalculateFitness();
		CalculateTreeSize();
	}
}
void Population::MutatePopulation() {
	for (auto p : pop_) {
		p.Mutate(mutation_rate_);
	}
}
size_t Population::SelectIndividual() {
	size_t winner;
	size_t challenger;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> d{ 0,pop_.size() - 1 };

	winner = d(mt);

	for (size_t i = 0; i < tournament_size_; ++i) {
		challenger = d(mt);
		if (pop_[challenger].GetFitness() < pop_[winner].GetFitness()) {
			winner = challenger;
		}
	}
	return winner;
}

void Population::Crossover(Node *father, Node *mother) {
	/* Randomly select a subtree from father and mother */
	/* Attach mother subtree to father subtree at father's crossover point */
	Node *offspring = new Node;
	offspring->Copy(father);

}
void Population::CalculateFitness() {
	double cur_fitness = 0;
	avg_fitness_ = 0;
	for (auto p : pop_) {
		p.CalculateFitness(solutions_);
		cur_fitness = p.GetFitness();
		avg_fitness_ += cur_fitness;
		if (cur_fitness < best_fitness_) {
			best_fitness_ = cur_fitness;
		} else if (cur_fitness > worst_fitness_) {
			worst_fitness_ = cur_fitness;
		}
	}
	avg_fitness_ = avg_fitness_ / pop_.size();
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
std::string Population::BestTreeToString() {
	return ToString(Elitism(2)[0]);
}
std::string Population::AllTreesToString() {
	std::string all_trees = "";
	for (auto p : pop_) {
		all_trees += p.ToString() + "\n";
	}
	return all_trees;
}
std::string Population::ToString(size_t tree_index) {
	return pop_[tree_index].ToString();
}
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
void Population::RunPrivateTestFunctions() {
	/* 
	 * This function is just to provide access to private functions
	 * from the main driver function while working different components
	 * of the project.
	 */
}