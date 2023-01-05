#include <vector>
#include <stack>
#include <map>

#include "cleantree.h"

std::vector<int> postorder(std::map<int, std::vector<int>>& adj, int start){
cleanTree(adj, start);
    std::vector<int> visited;
    std::vector<int> into{start};
    std::stack<int> stack;
    stack.push(start);

    while(!stack.empty()){
        int curr = stack.top();
        int count = 0;
        for(auto& n : adj[curr]){
            if(std::find(visited.begin(), visited.end(), n) != visited.end()){
                count++;
            }
        }
        bool visit = false;
        //if((adj[curr].size() != 1 && count == adj[curr].size() - 1) || count == adj[curr].size()){ visit = true; }
        if(count == adj[curr].size()){ 
            visited.push_back(curr);
        }{
            for(int i = adj[curr].size() - 1; i > -1; i--){
                if(std::find(into.begin(), into.end(), adj[curr][i]) == into.end()){ into.emplace_back(adj[curr][i]); stack.push(adj[curr][i]); stack.push(curr); }
            }
        }
        stack.pop();
    }

    return visited;
}