#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <deque>
#include <set>
#include <stack>
#include <queue>
#include <cmath>
#include <climits>
#include <unordered_set>

using namespace std;

const bool PRINT_MODE = false;

// USEFUL STRUCTURES AND HELPER FUNCTIONS:
//********************************************************************************************
//********************************************************************************************

// Vertex representing state of State space
struct Vertex {
    int x, y;

    Vertex(int xCoord, int yCoord) : x(xCoord), y(yCoord) {};
    Vertex() : x(0), y(0) {};

    bool operator ==(Vertex b) const {
        return x == b.x && y == b.y;
    }

    bool operator !=(Vertex b) const {
        return !(this->operator==(b));
    }

    bool operator<(const Vertex& other) const {
        return x == other.x ? y < other.y : x < other.x;
    }
};

//--------------------------------------------------------------------------------------------

// Hash function for Vertex
struct VertexHash {
    size_t operator()(const Vertex& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

//--------------------------------------------------------------------------------------------

// Euclidean distance, which is used as heuristic function for Greedy and A* algorithms
double getEuclidDst(const Vertex & a, const Vertex & b) {

    double square1 = pow(a.x - b.x, 2);
    double square2 = pow(a.y - b.y, 2);

    return sqrt(square1 + square2);
}

//--------------------------------------------------------------------------------------------

// Comparator for Greedy algorithm priority queue
struct EuclideanDstCmp {
    Vertex endVertex; // Endpoint for distance comparison

    EuclideanDstCmp(const Vertex &endVertex) : endVertex(endVertex) {}

    // Comparison function for priority queue
    bool operator()(const Vertex &a, const Vertex &b) const {
        double distanceA = getEuclidDst(a, endVertex);
        double distanceB = getEuclidDst(b, endVertex);

        return distanceA > distanceB;
    }
};

//--------------------------------------------------------------------------------------------

// Comparator for A* algorithm priority queue, which counts with h(x) and g(x)
struct AStarCmp {
    Vertex endVertex;
    const map<Vertex, int>& distance;

    AStarCmp(const Vertex &endVertex, const map<Vertex, int> &distance)
            : endVertex(endVertex), distance(distance) {}

    bool operator()(const Vertex &a, const Vertex &b) const {
        auto distA = distance.find(a);
        auto distB = distance.find(b);
        double distanceA = distA != distance.end() && distA->second != INT_MAX ? getEuclidDst(a, endVertex) + distA->second : std::numeric_limits<double>::infinity();
        double distanceB = distB != distance.end() && distB->second != INT_MAX ? getEuclidDst(b, endVertex) + distB->second : std::numeric_limits<double>::infinity();

        return distanceA > distanceB;
    }
};

//********************************************************************************************
//********************************************************************************************


// FUNCTIONS USED WITH STATE MATRIX
//********************************************************************************************
//********************************************************************************************

// Funtion used to write a char at the specific matrix position
void writeToMatrix(Vertex pos, char writeChar, vector<vector<char>> & stateMatrix) {

    if(stateMatrix[pos.y][pos.x] == 'S' || stateMatrix[pos.y][pos.x] == 'E') return;

    stateMatrix[pos.y][pos.x] = writeChar;
}

//--------------------------------------------------------------------------------------------

// Function used to print state matrix
void printMatrix(vector<vector<char>> & stateMatrix) {

    cout << endl;

    for (size_t i = 0; i < stateMatrix.size(); ++i) {
        for (size_t j = 0; j < stateMatrix[i].size(); ++j) {
            cout << stateMatrix[i][j];
        }
        cout << endl;
    }

    cout << endl;
}

//--------------------------------------------------------------------------------------------

// Function used to initialize the matrix before some algorithm starts
void initializeMatrix(vector<vector<char>> & stateMatrix, const string & path, Vertex s, Vertex e) {
    ifstream readLabyrinth(path);

    string line;
    while ( getline(readLabyrinth, line) ) {
        vector<char> row(line.begin(), line.end());
        stateMatrix.push_back(row);
    }

    stateMatrix.pop_back();
    stateMatrix.pop_back();

    stateMatrix[s.y][s.x] = 'S';
    stateMatrix[e.y][e.x] = 'E';

}

//--------------------------------------------------------------------------------------------

// Function used to print final state of matrix, including informations about expanded nodes and distance of path
void printFinalState(vector<vector<char>> & stateMatrix, int nodesExpanded, int distance) {
    cout << "FINAL: " << endl;
    cout << "******************************************" << endl;

    printMatrix(stateMatrix);

    cout << "******************************************" << endl;

    cout << "S Start" << endl;
    cout << "E End" << endl;
    cout << "# Opened node" << endl;
    cout << "o Path" << endl;
    cout << "X Wall" << endl;
    cout << "space Fresh node" << endl;

    cout << "******************************************" << endl;

    cout << "Cesta má délku: " << distance << endl;
    cout << "Expandované vrcholy: " << nodesExpanded << endl;
}


//********************************************************************************************
//********************************************************************************************


// FUNCTIONS USED TO LOAD LABYRINTH AND CHECK INPUT
//********************************************************************************************
//********************************************************************************************

// Function used to load labyrinth from text file and code it to the State space graph
map<pair<int, int>, vector<Vertex>> load_labyrinth(const string & path, Vertex & start, Vertex & end) {
    ifstream readLabyrinth(path);
    char c;

    set<Vertex> vertexes;

    int x = 0;
    int y = 0;

    // Read vertexes
    while ( readLabyrinth.get(c) ) {

        if(c == 's') break;

        if ( c == ' ' ) {
            Vertex v{x, y};
            vertexes.insert(v);
            x++;
        } else if (c == '\n') {
            x = 0;
            y++;
        } else {
            x++;
        }
    }

    vector<int> coords;
    string line;

    // Get start and end vertex
    while ( getline(readLabyrinth, line) ) {
        stringstream ss;
        ss << line;


        string temp;
        int found;
        while (!ss.eof()) {
            ss >> temp;
            if (stringstream(temp) >> found)
                coords.push_back(found);

            temp = "";
        }
    }

    start.x = coords[0];
    start.y = coords[1];

    end.x = coords[2];
    end.y = coords[3];

    map<pair<int, int>, vector<Vertex>> adjacencyList;

    for (auto & v : vertexes) {
        Vertex up = {v.x, v.y - 1};
        Vertex down = {v.x, v.y + 1};
        Vertex right = {v.x + 1, v.y};
        Vertex left = {v.x - 1, v.y};

        if(vertexes.find(up) != vertexes.end()) {
            adjacencyList[{v.x, v.y}].push_back(up);
        }

        if(vertexes.find(down) != vertexes.end()) {
            adjacencyList[{v.x, v.y}].push_back(down);
        }

        if(vertexes.find(right) != vertexes.end()) {
            adjacencyList[{v.x, v.y}].push_back(right);
        }

        if(vertexes.find(left) != vertexes.end()) {
            adjacencyList[{v.x, v.y}].push_back(left);
        }

    }

    return adjacencyList;
}

//--------------------------------------------------------------------------------------------

// Function used to check if the input from command line is correct
bool checkInput(int argc, char * argv[]) {
    string algorithms[5] = {"random", "bfs", "dfs", "greedy", "a"};

    if(argc != 3) {
        cout << "Špatný počet parametrů" << endl;
        cout << argv[0] << " [labyrinthPath] [algorithm]" << endl;
        return false;
    }

    ifstream readLabyrinth(argv[1]);

    if ( readLabyrinth.fail () ) {
        cout << "Špatná cesta k souboru." << endl;
        return false;
    }

    for(int i = 0; i < 5; i++) {
        if(algorithms[i] == argv[2]) {
            return true;
        }
    }

    cout << "Zadán špatný algoritmus, dostupné algoritmy jsou: random, bfs, dfs, greedy, a" << endl;
    return false;
}

//********************************************************************************************
//********************************************************************************************


// RECONSTRUCT PATH ALGORITHM WHICH IS COMMON FOR ALL ALGORITHMS
//********************************************************************************************
//********************************************************************************************

void reconstructPath(map<Vertex, Vertex> & prev, const Vertex & end,
                     vector<vector<char>> & stateMatrix) {
    Vertex a = end;
    while ( prev[a] != Vertex{-1, -1} ) {
        writeToMatrix(a, 'o', stateMatrix);
        a = prev[a];
    }

}

//********************************************************************************************
//********************************************************************************************


// A STAR ALGORITHM
//********************************************************************************************
//********************************************************************************************

void AStar(map<std::pair<int, int>, vector<Vertex>> &graph, const Vertex &start, const Vertex &end,
           vector<vector<char>> & stateMatrix) {
    vector<Vertex> queue;
    unordered_set<Vertex, VertexHash> closed;

    map<Vertex, int> distance;
    map<Vertex, Vertex> prev;

    int nodesExpanded = 0;

    for (const auto &node : graph) {
        distance[{node.first.first, node.first.second}] = INT_MAX;
    }

    distance[start] = 0;
    prev[start] = {-1, -1};

    queue.push_back(start);
    make_heap(queue.begin(), queue.end(), AStarCmp(end, distance));

    while (!queue.empty()) {
        pop_heap(queue.begin(), queue.end(), AStarCmp(end, distance));
        Vertex v = queue.back();
        queue.pop_back();
        nodesExpanded++;

        if (v == end) {
            reconstructPath(prev, end, stateMatrix);
            printFinalState(stateMatrix, nodesExpanded, distance[v]);
            break;
        }

        if (closed.find(v) != closed.end()) continue;
        closed.insert(v);

        bool somethingOpened = false;
        for (const auto &neighbour : graph[{v.x, v.y}]) {
            if (closed.find(neighbour) != closed.end()) continue;

            int gScore = distance[v] == INT_MAX ? INT_MAX : distance[v] + 1;

            if (gScore < distance[neighbour]) {
                prev[neighbour] = v;
                distance[neighbour] = gScore;

                if (find(queue.begin(), queue.end(), neighbour) == queue.end()) {
                    queue.push_back(neighbour);
                    push_heap(queue.begin(), queue.end(), AStarCmp(end, distance));

                    if(PRINT_MODE) writeToMatrix(neighbour, '#', stateMatrix);

                    somethingOpened = true;
                } else {
                    // Since we can't directly decrease the key in a heap, we make_heap to reorder it.
                    make_heap(queue.begin(), queue.end(), AStarCmp(end, distance));
                }
            }
        }

        if(somethingOpened && PRINT_MODE) {
            printMatrix(stateMatrix);
        }

    }
}

//********************************************************************************************
//********************************************************************************************


// GREEDY SEARCH ALGORITHM
//********************************************************************************************
//********************************************************************************************

void greedy(map<pair<int, int>, vector<Vertex>> & graph, const Vertex & start, const Vertex & end,
            vector<vector<char>> & stateMatrix) {

    EuclideanDstCmp comparator(end);
    unordered_set<Vertex, VertexHash> visited;

    priority_queue<Vertex, vector<Vertex>, EuclideanDstCmp> queue(comparator);
    queue.push(start);

    map<Vertex, int> distance;
    map<Vertex, Vertex> prev;

    int nodesExpanded = 0;

    distance[start] = 0;
    prev[start] = {-1 , -1};

    while ( !queue.empty() ) {
        Vertex v = queue.top();
        queue.pop();

        nodesExpanded++;

        if(v == end) {
            reconstructPath(prev, end, stateMatrix);
            printFinalState(stateMatrix, nodesExpanded, distance[v]);
            break;
        }

        bool somethingOpened = false;
        for (auto & neighbour : graph[{v.x, v.y}]) {
            if ( visited.find(neighbour) == visited.end() ) {
                somethingOpened = true;

                queue.push(neighbour);
                prev[neighbour] = v;
                distance[neighbour] = distance[v] + 1;

                if(PRINT_MODE) writeToMatrix(neighbour, '#', stateMatrix);
            }
        }
        visited.insert(v);

        if (somethingOpened && PRINT_MODE) {
            printMatrix(stateMatrix);
        }

    }

}

//********************************************************************************************
//********************************************************************************************


// DFS ALGORITHM
//********************************************************************************************
//********************************************************************************************
void dfs(map<pair<int, int>, vector<Vertex>> & graph, const Vertex & start, const Vertex & end,
         vector<vector<char>> & stateMatrix) {

    stack<Vertex> stack;
    unordered_set<Vertex, VertexHash> visited;

    stack.push(start);

    map<Vertex, int> distance;
    map<Vertex, Vertex> prev;

    int nodesExpanded = 0;

    distance[start] = 0;
    prev[start] = {-1 , -1};

    while ( !stack.empty() ) {
        Vertex v = stack.top();
        stack.pop();
        visited.insert(v);

        nodesExpanded++;

        if( v == end ) {

            reconstructPath(prev, end, stateMatrix);
            printFinalState(stateMatrix, nodesExpanded, distance[v]);
            break;
        }

        bool somethingOpened = false;
        for (auto & neighbour : graph[{v.x, v.y}]) {
            if (visited.find(neighbour) == visited.end()) {
                somethingOpened = true;
                stack.push(neighbour);
                prev[neighbour] = v;
                distance[neighbour] = distance[v] + 1;

                if(PRINT_MODE) writeToMatrix(neighbour, '#', stateMatrix);
            }
        }

        if(somethingOpened && PRINT_MODE) {
            printMatrix(stateMatrix);
        }
    }

}
//********************************************************************************************
//********************************************************************************************


// BFS ALGORITHM
//********************************************************************************************
//********************************************************************************************
void bfs(map<pair<int, int>, vector<Vertex>> & graph, const Vertex & start, const Vertex & end,
         vector<vector<char>> & stateMatrix) {

    deque<Vertex> opened;
    unordered_set<Vertex, VertexHash> closed;

    opened.push_back(start);

    map<Vertex, int> distance;
    map<Vertex, Vertex> prev;

    distance[start] = 0;
    prev[start] = {-1 , -1};

    int nodesExpanded = 0;

    while ( !opened.empty() ) {
        Vertex v = opened.front();
        opened.pop_front();

        nodesExpanded++;

        if ( v == end ) {

            reconstructPath(prev, end, stateMatrix);
            printFinalState(stateMatrix, nodesExpanded, distance[{v.x, v.y}]);
            break;
        }

        bool somethingOpened = false;
        for (auto & neighbour : graph[{v.x, v.y}]) {
            if ( closed.find(neighbour) == closed.end() &&
                 find(opened.begin(), opened.end(), neighbour) == opened.end()) {
                somethingOpened = true;

                opened.push_back(neighbour);
                distance[neighbour] = distance[v] + 1;
                prev[neighbour] = v;

                if(PRINT_MODE) writeToMatrix(neighbour, '#', stateMatrix);
            }
        }
        closed.insert(v);

        if(somethingOpened && PRINT_MODE) {
            printMatrix(stateMatrix);
        }

    }

}
//********************************************************************************************
//********************************************************************************************


// RANDOM SEARCH ALGORITHM
//********************************************************************************************
//********************************************************************************************
void randomSearch(map<pair<int, int>, vector<Vertex>> & graph, const Vertex & start, const Vertex & end,
                  vector<vector<char>> & stateMatrix) {
    unordered_set<Vertex, VertexHash> opened;
    unordered_set<Vertex, VertexHash> closed;

    map<Vertex, int> distance;
    map<Vertex, Vertex> prev;

    distance[start] = 0;
    prev[start] = {-1, -1};


    int nodesExpanded = 0;
    opened.insert(start);

    srand(time(NULL));

    while ( !opened.empty() ) {
        auto it = opened.begin();
        std::advance(it, rand() % opened.size());
        Vertex v = *it;

        opened.erase(it);

        nodesExpanded++;

        if ( v == end ) {

            reconstructPath(prev, end, stateMatrix);
            printFinalState(stateMatrix, nodesExpanded, distance[v]);
            break;
        }

        bool somethingOpened = false;
        for ( Vertex & neighbour : graph[{v.x, v.y}]) {
            if( closed.find(neighbour) == closed.end() && opened.find(neighbour) == opened.end()) {
                somethingOpened = true;

                opened.insert(neighbour);

                distance[neighbour] = distance[v] + 1;
                prev[neighbour] = v;

                if(PRINT_MODE) writeToMatrix(neighbour, '#', stateMatrix);
            }
        }

        if(somethingOpened && PRINT_MODE) {
            printMatrix(stateMatrix);
        }

        closed.insert(v);
    }

}

//********************************************************************************************
//********************************************************************************************


// MAIN FUNCTION
//********************************************************************************************
//********************************************************************************************
int main(int argc, char * argv[]) {

    if(!checkInput(argc, argv)) return EXIT_FAILURE;

    Vertex start;
    Vertex end;

    map<pair<int, int>, vector<Vertex>> adjacencyList;

    string path = argv[1];
    string algorithm = argv[2];
    std::transform(algorithm.begin(), algorithm.end(), algorithm.begin(), ::toupper);

    adjacencyList = load_labyrinth(path, start, end);
    vector<vector<char>> stateMatrix;

    initializeMatrix(stateMatrix, path, start, end);

    cout << "Starting algorithm " << algorithm << " for this labyrinth: " << endl;

    printMatrix(stateMatrix);

    if (algorithm == "RANDOM") {
        randomSearch(adjacencyList, start, end, stateMatrix);
    } else if (algorithm == "BFS") {
        bfs(adjacencyList, start, end, stateMatrix);
    } else if (algorithm == "DFS") {
        dfs(adjacencyList, start, end, stateMatrix);
    } else if (algorithm == "GREEDY") {
        greedy(adjacencyList, start, end, stateMatrix);
    } else if (algorithm == "A") {
        AStar(adjacencyList, start, end, stateMatrix);
    }

    return EXIT_SUCCESS;
}

//********************************************************************************************
//********************************************************************************************
