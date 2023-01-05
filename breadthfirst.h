#include <vector>
#include <queue>
#include <map>

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