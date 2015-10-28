/*
* ec-symbolicreg.cpp
* UIdaho CS-572: Evolutionary Computation
* Driver class for the Symbolic Regression Genetic Program
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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "individual.h"
#include "solution_data.h"

std::vector<SolutionData> ParseInput(std::string filename);

int main() {
	const size_t kTreeDepthMax = 3;
	const double kConstMin = -10.0f;
	const double kConstMax = 10.0f;
	std::vector<SolutionData> solutions(ParseInput("GPProjectData.csv"));
	Individual t(kTreeDepthMax, solutions[0].x.size(), kConstMin, kConstMax);
	t.CalculateFitness(solutions);

	return 0;
}

std::vector<SolutionData> ParseInput(std::string filename) {
	std::ifstream inf;
	std::string line;
	char delim = ',';
	size_t var_count = 0;
	size_t line_count = 0;
	std::vector<SolutionData> solutions;

	inf.open(filename, std::ifstream::in);

	if (!inf) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Get number of lines in file */
	size_t lc = 0;
	std::string tmp;
	while (std::getline(inf, tmp))
		++lc;
	inf.clear();
	inf.seekg(0, std::ios::beg);

	/* Parse input file */
	solutions.resize(lc - 1);
	std::getline(inf, line); /* First line is CSV headers */
	var_count = std::count(line.begin(), line.end(), delim);
	while (std::getline(inf, line)) {
		size_t pos = 0;
		solutions[line_count].x.reserve(var_count);
		std::string tok;
		for (size_t cell = 0; cell < var_count; ++cell) {
			pos = line.find(delim);
			solutions[line_count].x.push_back(std::stod(line.substr(0, pos)));
			line.erase(0, pos + 1); /* 1 is length of delim */
		}
		solutions[line_count].x.shrink_to_fit();
		solutions[line_count].y = std::stod(line);
		++line_count;
	}
	solutions.shrink_to_fit();
	inf.close();
	return solutions;
}