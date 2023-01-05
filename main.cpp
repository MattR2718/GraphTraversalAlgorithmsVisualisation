#include <iostream>
#include <vector>
#include <tuple>
#include <filesystem>
#include <string>
#include <regex>
#include <map>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <implot.h>

#include "scrollingBuffer.h"

#include "breadthfirst.h"
#include "depthfirst.h"
#include "preorder.h"
#include "inorder.h"
#include "postorder.h"

#define COOLDOWN_LIMIT 25
#define VISIT_RATE 25

int insideNode(std::vector<std::tuple<int, int, int>>& nodes, sf::Vector2i& pos, float& r, bool draw) {
    for (auto& node : nodes) {
        float xDist = std::get<1>(node) - pos.x;
        float yDist = std::get<2>(node) - pos.y;
        float magDist = sqrt((xDist * xDist) + (yDist * yDist));
        if ((draw) && (magDist <= r * 2)) {
            return std::get<0>(node);
        }
        else if (magDist <= r) {
            return std::get<0>(node);
        }
    }
    return -1;
}

sf::Color makeColour(float colour[3]) {
    return (sf::Color((int)(colour[0] * 255), (int)(colour[1] * 255), (int)(colour[2] * 255)));
}

void cleanList(std::map<int, std::vector<int>>& adjacencyList, int& nodeIn, std::vector<std::tuple<int, int, int>> nodes) {
    adjacencyList.erase(nodeIn);
    for (auto& node : nodes) {
        if (std::get<0>(node) == nodeIn) {
            if (adjacencyList[std::get<0>(node)][0] == nodeIn) {
                adjacencyList[std::get<0>(node)].erase(adjacencyList[std::get<0>(node)].begin());
            }
            else if ((adjacencyList[std::get<0>(node)].size() == 2) && (adjacencyList[std::get<0>(node)][1] == nodeIn)) {
                adjacencyList[std::get<0>(node)].erase(adjacencyList[std::get<0>(node)].begin() + 1);
            }
        }
    }
}

void connectNodes(int connect[2], std::map<int, std::vector<int>>& adjacencyList) {
    if(connect[0] == -1 | connect[1] == -1){ return; }
    if(connect[0] == connect[1]){ return; }
    if(std::find(adjacencyList[connect[0]].begin(), adjacencyList[connect[0]].end(), connect[1]) != adjacencyList[connect[0]].end()){ return; }
    // Make checks for more than two connections and being connected to by more than one
    adjacencyList[connect[0]].push_back(connect[1]);
    adjacencyList[connect[1]].push_back(connect[0]);
}

std::pair<int, int> getCoords(std::vector<std::tuple<int, int, int>>& nodes, int& nodeNum) {
    for (auto& node : nodes) {
        if (std::get<0>(node) == nodeNum) {
            return std::make_pair(std::get<1>(node), std::get<2>(node));
        }
    }
    return std::make_pair(-1, -1);
}

void reset(std::vector<std::tuple<int, int, int>>& nodes, std::map<int, std::vector<int>>& adjacencyList, int& nodeIndex) {
    nodes = std::vector<std::tuple<int, int, int>>{};
    adjacencyList = std::map<int, std::vector<int>>{};
    nodeIndex = 0;
}

void printAdj(std::map<int, std::vector<int>>& adj){
    std::cout<<"--------------------------\n";
    for(auto& a : adj){
        std::cout<<a.first<<": ";
        for(auto& s : a.second){ std::cout<<s<<' '; }
        std::cout<<'\n';
    }
    std::cout<<'\n';
    std::cout<<"--------------------------\n";

}

void loadFiles(std::vector<std::string>& files){
    files.clear();
    std::regex ex("[.]*\\\\([a-zA-z]+).txt");
    std::smatch m;
    for(const auto& file : std::filesystem::directory_iterator("../graphs")){
        std::string str = file.path().string();
        std::string fileString = "";
        while (std::regex_search(str, m, ex)) {
            fileString += m[1];
            str = m.suffix().str();
        }
        files.push_back(fileString);
    }
}

void loadGraph(const std::vector<std::string>& files, const int selectedIndex, std::vector<std::tuple<int, int, int>>& nodes, std::map<int, std::vector<int>>& adjacencyList, int& start){
    std::ifstream file("../graphs/" + files[selectedIndex] + ".txt");
    std::string linetxt;
    bool gettingNodes = true;
    bool gettingAdj = false;
    bool getStart = false;
    nodes.clear();
    adjacencyList.clear();
    while(std::getline(file, linetxt)){
        if(linetxt.length() == 0){
            if(gettingNodes){ gettingNodes = false; gettingAdj = true; continue;}
            if(gettingAdj){ gettingAdj = false; getStart = true; continue;}
        }else{
            if(gettingNodes){
                std::stringstream ss(linetxt);
                int i, x, y;
                ss >> i >> x >> y;
                std::tuple<int, int, int> n{i, x, y};
                nodes.emplace_back(n);
            }else if(gettingAdj){
                std::vector<int> nums;
                std::stringstream ss(linetxt);
                std::string token;
                while(std::getline(ss, token, ' ')){
                    nums.emplace_back(std::stoi(token));
                }
                adjacencyList[nums[0]];
                for(int i = 1; i < nums.size(); i++){
                    adjacencyList[nums[0]].emplace_back(nums[i]);
                }
            }else{
                start = std::stoi(linetxt);
            }
        }
    }
}

int main()
{
    constexpr int WIDTH = 1600;
    constexpr int HEIGHT = 950;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Graph Traversal Algorithms");
    window.setFramerateLimit(120);
    window.setPosition(sf::Vector2i(0, 0));

    auto image = sf::Image{};
    //https://www.flaticon.com/authors/dreamstale
    if (!image.loadFromFile("../imgs/greengraph.png"))
    {
        // Error handling...
    }
    window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
    auto _ = ImGui::SFML::Init(window);
    ImGui::CreateContext();
    ImPlot::CreateContext();

    //                     <id,  x,   y>
    std::vector<std::tuple<int, int, int>> nodes;
    int nodeIndex = 0;
    std::map<int, std::vector<int>> adjacencyList;
    std::vector<int> visited;

    bool showNodes = true;
    float nodeRadius = 50.0f;
    int circleSegments = 100;
    float nodeColour[3] = { (float)204 / 255, (float)77 / 255, (float)5 / 255 };
    float bgColour[3] = { (float)18 / 255, (float)33 / 255, (float)43 / 255 };
    float edgeColour[3] = { (float)255 / 255, (float)255 / 255, (float)255 / 255 };
    float selectColour[3] = { (float)50 / 255, (float)255 / 255, (float)50 / 255 };
    float visitColour[3] = { (float)0 / 255, (float)17 / 255, (float)191 / 255 };
    float startColour[3] = { (float)255 / 255, (float)255 / 255, (float)255 / 255 };

    float frameHist[1000] = {0};
    ScrollingBuffer frameData;

    std::vector<std::string> files;
    loadFiles(files);

    int startNode = 0;

    int coolDown = 0;
    int upto = 0;
    int at = 0;
    bool running = false;
    int connect[2] = { -1, -1 };
    std::pair<int, int> nullCoords = { -1, -1 };

    int selectedIndex = 0;
    std::string saveFileName{""};
    std::map<std::string, std::pair<std::vector<std::tuple<int, int, int>>, std::map<int, std::vector<int>>>> saved;

    sf::Clock fpsClock;
    float time = 0;
    static float plott = 0;

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if ((!running) && (coolDown > COOLDOWN_LIMIT) && (sf::Mouse::isButtonPressed(sf::Mouse::Right))) {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                if (insideNode(nodes, pos, nodeRadius, true) == -1) {
                    std::tuple<int, int, int> temp = { nodeIndex, pos.x, pos.y};
                    nodeIndex++;
                    nodes.push_back(temp);
                }
                else {
                    int nodeIn = insideNode(nodes, pos, nodeRadius, false);
                    if (nodeIn > -1) {
                        for (int i = 0; i < nodes.size(); i++) {
                            if (nodeIn == std::get<0>(nodes[i])) {
                                nodes.erase(nodes.begin() + i);
                                cleanList(adjacencyList, nodeIn, nodes);
                                upto = 0;
                            }
                        }
                    }
                }
                coolDown = 0;
            }
            else if ((!running) && (coolDown > COOLDOWN_LIMIT) && (sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                int nodeIn = insideNode(nodes, pos, nodeRadius, false);
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
                    startNode = nodeIn;
                    at = startNode;
                }else{
                    if (connect[0] == -1) {
                        connect[0] = nodeIn;
                    }
                    else if (connect[1] == -1) {
                        connect[1] = nodeIn;
                        connectNodes(connect, adjacencyList);
                        connect[0] = -1;
                        connect[1] = -1;
                    }
                }
            }
        }

        coolDown++;

        ImGui::SFML::Update(window, deltaClock.restart());

        //ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
        ImGui::Begin("##", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(0, 0));
        

        if(ImGui::CollapsingHeader("Instructions", ImGuiTreeNodeFlags_DefaultOpen)){
            ImGui::BulletText("RClick to add and remove nodes");
            ImGui::BulletText("LClick to select nodes");
            ImGui::Indent();
            ImGui::BulletText("Select two nodes to connect them");
            ImGui::Unindent();
            ImGui::BulletText("CTRL+LClick to set start node");
        }

        if(ImGui::CollapsingHeader("Algorithms", ImGuiTreeNodeFlags_DefaultOpen)){
            const char* items[] = { "Depth First", "Breadth First", "Preorder", "Inorder", "Postorder"};
            static int item_current = 0;
            ImGui::ListBox("##", &item_current, items, IM_ARRAYSIZE(items), (int)(sizeof(items) / sizeof(*items)));
            if (ImGui::Button("Traverse")) {
                upto = 0;
                running = true;
                visited.clear();
                switch (item_current) {
                case 0:
                    visited = depthFirst(adjacencyList, startNode);
                    break;
                case 1:
                    visited = breadthFirst(adjacencyList, startNode);
                    break;
                case 2:
                    visited = preorder(adjacencyList, startNode);
                    break;
                case 3:
                    visited = inorder(adjacencyList, startNode);
                    break;
                case 4:
                    visited = postorder(adjacencyList, startNode);
                    break;
                }
            }
            if (ImGui::Button("Stop")) { running = false; }
        }

        if(ImGui::CollapsingHeader("Load/Save", ImGuiTreeNodeFlags_DefaultOpen)){
            if (ImGui::BeginCombo("##Files", files[selectedIndex].c_str())) {
                for (int i = 0; i < files.size(); ++i) {
                    const bool isSelected = (selectedIndex == i);
                    if (ImGui::Selectable(files[i].c_str(), isSelected)) {
                        selectedIndex = i;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }


            if(ImGui::Button("Update")){
                loadFiles(files);
            }

            if(ImGui::Button("Save")){
                ImGui::OpenPopup("Save Graph");
            }
            if(ImGui::BeginPopupModal("Save Graph", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
                char buf[255]{};
                strncpy_s( buf, saveFileName.c_str(), sizeof(buf)-1 );
                ImGui::InputText( "Enter Graph Name... ", buf, sizeof(buf) );
                saveFileName = buf;
                saveFileName.erase(remove_if(saveFileName.begin(), saveFileName.end(), isspace), saveFileName.end());
                std::string path = "../graphs/" + saveFileName + ".txt";
                if(ImGui::Button("Save")){
                    std::ofstream saveFile(path);
                    for(auto& n : nodes){
                        saveFile << std::get<0>(n) << ' ' << std::get<1>(n) << ' ' << std::get<2>(n) << '\n';
                    }
                    saveFile << '\n';
                    for(auto& a : adjacencyList){
                        saveFile << a.first << ' ';
                        for(auto& n : a.second){
                            saveFile << n << ' ';
                        }
                        saveFile << '\n';
                    }
                    saveFile << '\n' << startNode;
                    saveFile.close();
                    saveFileName = "";
                    loadFiles(files);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if(ImGui::Button("Load")){
                loadGraph(files, selectedIndex, nodes, adjacencyList, startNode);
                at = 0;
                upto = 0;
                nodeIndex = nodes.size();
            }
            
        }

        if(ImGui::CollapsingHeader("Customise", ImGuiTreeNodeFlags_DefaultOpen)){
            ImGui::Text("Number of nodes: %i", nodes.size());
            ImGui::Checkbox("Show Nodes", &showNodes);
            ImGui::SliderFloat("Node Size", &nodeRadius, 0.0f, 100.0f);
            ImGui::SliderInt("Sides", &circleSegments, 3, 150);
            ImGui::ColorEdit3("Node Colour", nodeColour);
            ImGui::ColorEdit3("Edge Colour", edgeColour);
            ImGui::ColorEdit3("Selected Colour", selectColour);
            ImGui::ColorEdit3("Background Colour", bgColour);
            ImGui::ColorEdit3("Visiting Colour", visitColour);
            ImGui::ColorEdit3("Start Colour", startColour);
            if (ImGui::Button("Clear All Nodes")) {
                reset(nodes, adjacencyList, nodeIndex);
                at = 0;
                upto = 0;
                running = false;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(3);

        ImGui::Begin("Frame Rate", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowPos(ImVec2(WIDTH - 800, 0));
        ImGui::SetWindowSize(ImVec2(400, 170));
        time = fpsClock.getElapsedTime().asSeconds();
        time = 1.0 / time;
        fpsClock.restart();
        plott += ImGui::GetIO().DeltaTime;
        frameData.AddPoint(plott, time);
        static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
        if(ImPlot::BeginPlot("##", ImVec2(-1, 150))){
            ImPlot::SetupAxes(NULL, NULL, flags, flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, plott - 10, plott, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1,0,200);
            ImPlot::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 1, 1));
            ImPlot::PlotLine(std::to_string(time).c_str(), &frameData.Data[0].x, &frameData.Data[0].y, frameData.Data.size(), 0, frameData.Offset, 2*sizeof(float));
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }
        ImGui::End();

        ImGui::Begin("Adjacency List", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowPos(ImVec2(WIDTH - 400, 0));
        ImGui::SetWindowSize(ImVec2(400, HEIGHT));
        std::string s = "";
        for(auto& a : adjacencyList){
            s += std::to_string(a.first) + ": ";
            for(auto& d : a.second){
                s += std::to_string(d) + " ";
            }
            s += "\n";
        }
        ImGui::SetWindowFontScale(3.0);
        ImGui::Text("%s", s.c_str());
        ImGui::End();

        window.clear(makeColour(bgColour));

        sf::CircleShape shape;

        //Draw Visiting
        if (running) {
            at++;
            if (at == VISIT_RATE) {
                at = 0;
                upto++;
            }
            if (upto == visited.size()) { upto = 0; }
            float r = (float)nodeRadius * 1.2;
            shape.setFillColor(makeColour(visitColour));
            shape.setOrigin(r, r);
            shape.setRadius(r);
            shape.setPointCount(circleSegments);
            for (auto& node : nodes) {
                for (int i = 0; i < upto + 1; i++) {
                    if (std::get<0>(node) == visited[i]) {
                        shape.setPosition(std::get<1>(node), std::get<2>(node));
                        window.draw(shape);
                        break;
                    }
                }
            }
            
        }

        //Draw Selected
        if (!running) {
            if (connect[0] != -1) {
                float r = (float)nodeRadius * 1.2;
                shape.setFillColor(makeColour(selectColour));
                shape.setOrigin(r, r);
                shape.setRadius(r);
                for (auto& node : nodes) {
                    if (std::get<0>(node) == connect[0]) {
                        shape.setPosition(std::get<1>(node), std::get<2>(node));
                    }
                }
                window.draw(shape);
            }
            if (connect[1] != -1) {
                for (auto& node : nodes) {
                    if (std::get<0>(node) == connect[1]) {
                        shape.setPosition(std::get<1>(node), std::get<2>(node));
                    }
                }
                window.draw(shape);
            }
        }

        //Draw connections
        for (auto& node : nodes) {
            int nodeNum = std::get<0>(node);
            std::pair<int, int> nodeCoords = getCoords(nodes, nodeNum);
            for (auto& n : adjacencyList[nodeNum]) {
                std::pair<int, int> node2Coords = getCoords(nodes, n);
                if (node2Coords != nullCoords) {
                    sf::Vertex line[] =
                    {
                        sf::Vertex(sf::Vector2f(nodeCoords.first, nodeCoords.second)),
                        sf::Vertex(sf::Vector2f(node2Coords.first, node2Coords.second))
                    };
                    line[0].color = makeColour(edgeColour);
                    line[1].color = makeColour(edgeColour);

                    window.draw(line, 2, sf::Lines);
                }
            }
        }

        //Draw Nodes
        for (auto& node : nodes) {
            shape.setRadius(nodeRadius);
            shape.setOrigin(nodeRadius, nodeRadius);
            shape.setPosition(std::get<1>(node), std::get<2>(node));
            shape.setPointCount(circleSegments);
            if(std::get<0>(node) == startNode){
                shape.setFillColor(makeColour(startColour));
            }else{
                shape.setFillColor(makeColour(nodeColour));
            }
            if (showNodes) {
                window.draw(shape);
            }
        }
        
        ImGui::SFML::Render(window);
        window.display();
    }
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    ImGui::SFML::Shutdown();
}
