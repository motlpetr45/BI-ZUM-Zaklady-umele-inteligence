#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

double getRandomDouble() {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> distr(0.0, 1.0);

    return distr(gen);
}

//--------------------------------------------------------------------------------------------------------

class ChessBoard {
public:
    ChessBoard(int N) : m_N(N) {
        m_Queens.resize(N);
        srand(time(0));
        PRINT_MODE = true;
    }

    void simulatedAnnealing();
    int computeConflicts();
    double computeProba(int fx, int fy, double t);
    void initPositions();

    void printSquare(bool isBlack, bool hasQueen);
    void print(unsigned long step, int conflicts, double temp);

private:
    int m_N;
    bool PRINT_MODE;

    // Index represents column and value represents row
    vector<int> m_Queens;

};

//--------------------------------------------------------------------------------------------------------

int ChessBoard::computeConflicts() {
    int conflicts = 0;

    for (int i = 0; i < m_N; ++i) {
        for (int j = i + 1; j < m_N; ++j) {

            // Row conflict
            if(m_Queens[i] == m_Queens[j]) {
                conflicts++;
            }

            // Diagonal conflict
            // If distance between queens in row and distance between queens in columns are equal
            if(abs(m_Queens[i] - m_Queens[j]) == abs(i - j)) {
                conflicts++;
            }

        }
    }

    return conflicts;

}

//--------------------------------------------------------------------------------------------------------

void ChessBoard::initPositions() {

    for (int i = 0; i < m_N; ++i) {
        m_Queens[i] = (rand() % m_N);
    }

}

//--------------------------------------------------------------------------------------------------------

double ChessBoard::computeProba(int fx, int fy, double t) {
    int delta = fy - fx;

    return exp(-delta / t);
}

//--------------------------------------------------------------------------------------------------------

void ChessBoard::simulatedAnnealing() {
    double temp = 30000;
    double coolingRate = 0.95;

    // Iterations without improvement before termination
    int stagnationLimit = 5000;

    // Track last improvement
    int lastImprovement = 0;

    initPositions();
    int conflicts = computeConflicts();
    unsigned long step = 0;

    while (conflicts > 0 && lastImprovement < stagnationLimit) {
        size_t queen = (rand() % m_N);
        int pos = (rand() % m_N);
        int oldPos = m_Queens[queen];

        m_Queens[queen] = pos;
        int newConflicts = computeConflicts();

        if(newConflicts >= conflicts) {
            lastImprovement++;
        } else {
            lastImprovement = 0;
        }

        if((newConflicts > conflicts) && (computeProba(conflicts, newConflicts, temp) < getRandomDouble()) ) {
            m_Queens[queen] = oldPos;
        } else {
            conflicts = newConflicts;
        }

        temp = temp * coolingRate;
        step++;

        if(PRINT_MODE) {
            // system("clear");
            cout << "\033[H";
            print(step, conflicts, temp);
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }

    if(!PRINT_MODE) {
        print(step, conflicts, temp);
    }

}

//--------------------------------------------------------------------------------------------------------

void ChessBoard::printSquare(bool isBlack, bool hasQueen) {
    string blackBg = "\033[40m";
    string whiteBg = "\033[47m";

    string queenColor = "\033[93m";
    string bold = "\033[1m";
    string reset = "\033[0m";

    string bgColor = isBlack ? blackBg : whiteBg;

    cout << bgColor << bold;
    if (hasQueen) {
        cout << queenColor << " Q" << reset << bgColor << " ";
    } else {
        cout << "   ";
    }
    cout << reset;
}

//--------------------------------------------------------------------------------------------------------

void ChessBoard::print(unsigned long step, int conflicts, double temp) {
    string bold = "\033[1m";
    string reset = "\033[0m";

    cout << bold;
    cout << "STEP: " << step << endl;
    cout << "Conflicts: " << conflicts << "               " << endl;
    cout << "Temperature: " << temp <<    "               " << endl;

    cout << reset;

    for (int i = 0; i < m_N; ++i) {
        for (int j = 0; j < m_N; ++j) {
            bool isBlack = (i + j) % 2 == 0;
            bool hasQueen = m_Queens[i] == j;
            printSquare(isBlack, hasQueen);
        }
        cout << endl;
    }
}

//--------------------------------------------------------------------------------------------------------

int main ( int argc, char ** argv ) {

    cout << "Please enter the chessboard size" << endl;

    if(argc != 2) {
        cout << argv[0] << " [chessboardSize]" << endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < atoi(argv[1]) + 10; ++i) {
        cout << endl;
    }

    cout << "\033[?25l";

    ChessBoard c = {atoi(argv[1])};
    c.simulatedAnnealing();

    cout << "\033[?25h";

    return EXIT_SUCCESS;
}
