#include "algorithms.h"

/*
void printMap(std::map<int, std::vector<int>>& adjacencyList) {
    for (auto& e = adjacencyList.cbegin(); e != adjacencyList.cend(); e++) {
        if (e->first) {
            std::cout << e->first << '\n';
        }
    }
}*/

int findNeighboursDF(int& c, std::vector<int>& visited, std::map<int, std::vector<int>>& adjacencyList) {
    /*std::cout << c << '\n';
    if (c == 0) {
        std::cout << "0 IN NEIGHBOURS\n";
    }*/
    //std::cout << c << '\n';
    for (auto& n : adjacencyList[c]) {
        if (!std::count(visited.begin(), visited.end(), n)) {
            return n;
            //std::cout << "N " << n << '\n';
        }
        //std::cout << "LOOP\n";
    }
    //std::cout << "DOESN'T EXIST\n";
    return -1;
}

std::vector<int> depthFirst(std::map<int, std::vector<int>>& adjacencyList, int start)
{
    std::vector<int> visited;
    std::stack<int> stack;
    visited.push_back(start);
    stack.push(start);

    while (stack.size() > 0) {
        int nextNode = findNeighboursDF(stack.top(), visited, adjacencyList);
        std::cout << nextNode << '\n';
        if (nextNode != -1) {
            visited.push_back(nextNode);
            stack.push(nextNode);
        }
        else {
            stack.pop();
        }
        //std::cout << stack.size() << '\n';
        //std::cout << visited.size() << '\n';
        //std::cout << stack.top();
    }

	return visited;
}



std::vector<int> findNeighboursBF(int& c, std::vector<int>& visited, std::map<int, std::vector<int>>& adjacencyList) {
    std::vector<int> n;
    for (auto& g : adjacencyList[c]) {
        if (!std::count(visited.begin(), visited.end(), g)) {
            n.push_back(g);
        }
    }
    return n;
}

std::vector<int> breadthFirst(std::map<int, std::vector<int>>& adjacencyList, int start)
{
    std::vector<int> visited;
    std::queue<int> queue;
    queue.push(start);

    while (visited.size() < adjacencyList.size()) {
        int node = queue.front();
        queue.pop();
        visited.push_back(node);
        std::vector<int> neighbours = findNeighboursBF(node, visited, adjacencyList);
        for (auto& n : neighbours) {
            queue.push(n);
        }

    }

	return visited;
}

std::vector<int> preOrder(std::map<int, std::vector<int>>& adjacencyList, int start, std::vector<int>& visited) {
    //std::cout << "PREORDER\n";
    //std::cout << adjacencyList[start][0] << '\n';
    if (!std::count(visited.begin(), visited.end(), start)) {
        visited.push_back(start);
    }
    if (adjacencyList[start].size() >= 1) {
        preOrder(adjacencyList, adjacencyList[start][0], visited);
    }
    if (adjacencyList[start].size() == 2) {
        preOrder(adjacencyList, adjacencyList[start][1], visited);
    }

    
    return visited;
}


//TODO
//If root node deleted, traversal does not work as assuming rootIndex 0 as root which can be deleted