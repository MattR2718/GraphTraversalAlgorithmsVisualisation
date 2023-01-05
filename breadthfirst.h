#include <vector>
#include <queue>
#include <map>

std::vector<int> breadthFirst(std::map<int, std::vector<int>>& adj, int start)
{
    std::vector<int> visited;
    std::queue<int> q;
    q.push(start);

    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        visited.push_back(curr);
        for (auto& n : adj[curr]) {
            if(std::find(visited.begin(), visited.end(), n) == visited.end()){ q.push(n); }
        }

    }

	return visited;
}