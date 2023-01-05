#include <vector>
#include <map>
#include <stack>

std::vector<int> depthFirst(std::map<int, std::vector<int>>& adj, int start)
{
    std::vector<int> visited;
    std::stack<int> stack;
    visited.emplace_back(start);
    stack.push(start);

    while (stack.size() > 0) {
        int curr = stack.top();
        stack.pop();

        for(auto& s : adj[curr]){ 
            if(std::find(visited.begin(), visited.end(), s) == visited.end()){ stack.push(s); }
        }
        visited.push_back(curr);

    }

	return visited;
}