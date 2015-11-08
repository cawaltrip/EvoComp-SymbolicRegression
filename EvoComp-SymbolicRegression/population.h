/*
* population.h
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
#pragma once

#include <vector>
#include "individual.h"

class Population {
public:
	Population(size_t population_size, double mutation_rate,
			   double nonterminal_crossover_rate, size_t tournament_size, 
			   size_t depth_min, size_t depth_max,
			   double const_min, double const_max, 
			   size_t var_count, std::vector<SolutionData> solutions);
	
	/* Helper Functions */
	std::string ToString(bool include_fitness = false);
	std::string GetBestSolutionToString(bool include_fitness = false);
	std::string GetBestWeightedSolutionToString(bool include_fitness = false);

	/* Public Genetic Program Functions */
	void Evolve(size_t elitism_count = 2);
	
	/* Private Accessor Functions */
	size_t GetLargestTreeSize();
	size_t GetSmallestTreeSize();
	size_t GetAverageTreeSize();
	size_t GetTotalNodeCount();
	double GetBestFitness();
	double GetWorstFitness();
	double GetAverageFitness();
	double GetBestWeightedFitness();
	double GetWorstWeightedFitness();
	double GetAverageWeightedFitness();
private:
	/* Private Genetic Program Functions */
	void RampedHalfAndHalf(size_t population_size,
						   size_t depth_min, size_t depth_max);
	void MutatePopulation();
	void Crossover(Individual *parent1, Individual *parent2);

	/* Helper functions */
	size_t SelectIndividual();
	std::vector<size_t> Elitism(size_t elite_count);
	void CalculateFitness();
	void CalculateRawFitness();
	void CalculateWeightedFitness();
	double CalculateParsimonyCoefficient();
	void CalculateTreeSize();

	/* Population Data */
	std::vector<Individual> pop_;
	std::vector<SolutionData> solutions_;
	double const_min_;
	double const_max_;
	size_t var_count_;
	double mutation_rate_;
	double nonterminal_crossover_rate_;
	size_t tournament_size_;

	/* Population Metadata */
	size_t largest_tree_;
	size_t smallest_tree_;
	size_t avg_tree_;
	size_t total_nodes_;
	size_t best_index_;
	size_t best_weighted_index_;
	double best_fitness_;
	double worst_fitness_;
	double avg_fitness_;
	double best_weighted_fitness_;
	double worst_weighted_fitness_;
	double avg_weighted_fitness_;
};