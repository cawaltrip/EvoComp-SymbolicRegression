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

Population::Population(size_t population_size,
	size_t depth_min, size_t depth_max,
	double const_min, double const_max, size_t var_count,
	std::vector<SolutionData> solutions) {

	solutions_ = solutions;

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
	RampedHalfAndHalf(population_size, depth_min, depth_max);
}
void Population::RampedHalfAndHalf(size_t population_size, 
	size_t depth_min, size_t depth_max) {
	unsigned gradations = depth_max - depth_min + 1;

	pop_.reserve(population_size);
	for (size_t i = 0; i < population_size; ++i) {
		pop_.push_back(Individual((depth_min + i % gradations) , var_count_, 
			const_min_, const_max_, static_cast<bool>(i%2)));
	}
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
		std::clog << "Current Fitness: " << cur_fitness << "\n";
	}
	avg_fitness_ = avg_fitness_ / pop_.size();
}
void Population::CalculateTreeSize() {
	size_t cur_tree = 0;
	avg_tree_ = 0;
	for (auto p : pop_) {
		p.CalculateTreeSize();
		cur_tree = p.GetTreeSize();
		avg_tree_ += cur_tree;
		if (cur_tree > largest_tree_) {
			largest_tree_ = cur_tree;
		} else if (cur_tree < smallest_tree_) {
			smallest_tree_ = cur_tree;
		}
	}
	avg_tree_ = avg_tree_ / pop_.size(); /* Will truncate, I don't care */
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

double Population::GetBestFitness() {
	return best_fitness_;
}
double Population::GetWorstFitness() {
	return worst_fitness_;
}
double Population::GetAverageFitness() {
	return avg_fitness_;
}