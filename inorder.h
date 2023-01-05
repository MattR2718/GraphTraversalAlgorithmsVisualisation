#include <vector>
#include <stack>
#include <map>

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

std::vector<int> inorder(std::map<int, std::vector<int>>& adj, int start){
    cleanTree(adj, start);
    std::vector<int> visited;
    std::vector<int> into{start};
    std::stack<int> stack;
    stack.push(start);
    int from = -1;

    while(!stack.empty()){
        int curr = stack.top();
        int count = 0;
        for(auto& n : adj[curr]){
            if(std::find(visited.begin(), visited.end(), n) != visited.end()){
                count++;
            }
        }
        bool visit = false;
        if((adj[curr].size() != 1 && count == adj[curr].size() - 1) || count == adj[curr].size()){ visit = true; }
        if(adj[curr].size() == 0 || visit){ 
            visited.push_back(curr);
        }{
            for(int i = adj[curr].size() - 1; i > -1; i--){
                if(std::find(into.begin(), into.end(), adj[curr][i]) == into.end()){ into.emplace_back(adj[curr][i]); stack.push(adj[curr][i]); stack.push(curr); }
            }
        }
        stack.pop();
        from = curr;
    }

    return visited;
}