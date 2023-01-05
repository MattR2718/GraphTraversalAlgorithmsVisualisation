#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <queue>

std::vector<int> depthFirst(std::map<int, std::vector<int>>& adjacencyList, int start);
std::vector<int> breadthFirst(std::map<int, std::vector<int>>& adjacencyList, int start);
std::vector<int> preOrder(std::map<int, std::vector<int>>& adjacencyList, int start, std::vector<int>& visited);


#endif