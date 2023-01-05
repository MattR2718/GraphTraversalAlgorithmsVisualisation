#ifndef CLEANTREE_H
#define CLEANTREE_H
#include <queue>

void cleanTree(std::map<int, std::vector<int>>& adj, int start){
    std::vector<int> visited;
    std::queue<int> q;
    std::map<int, int> prev;
    prev[start] = -1;
    q.push(start);

    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        visited.push_back(curr);
        for (auto& n : adj[curr]) {
            if(std::find(visited.begin(), visited.end(), n) == visited.end()){ prev[n] = curr; q.push(n); }
        }
    }

    for(auto& a : adj){
        for(int i = 0; i < a.second.size(); i++){
            if(a.second[i] == prev[a.first]){ a.second.erase(a.second.begin() + i); i--; }
        }
    }
}
#endif