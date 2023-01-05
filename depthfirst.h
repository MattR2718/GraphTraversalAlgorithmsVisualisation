#ifndef DEPTHFIRST_H
#define DEPTHFIRST_H

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

        for(int i = adj[curr].size() - 1; i > -1; i--){
            if(std::find(visited.begin(), visited.end(), adj[curr][i]) == visited.end()){ stack.push(adj[curr][i]); }
        }
        visited.push_back(curr);
    }

	return visited;
}

#endif