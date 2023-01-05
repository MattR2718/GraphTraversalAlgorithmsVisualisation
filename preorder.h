#include <vector>
#include <stack>
#include <map>

std::vector<int> preorder(std::map<int, std::vector<int>>& adj, int start){
    std::stack<int> stack;
    std::vector<int> visited;
    stack.push(start);
    //visited.emplace_back(start);
    while(!stack.empty()){
        int curr = stack.top();
        stack.pop();
        visited.emplace_back(curr);
        for(int i = adj[curr].size() - 1; i > -1; i--){
            if(std::find(visited.begin(), visited.end(), adj[curr][i]) == visited.end()){ stack.push(adj[curr][i]); }
        }
    }

    return visited;
}