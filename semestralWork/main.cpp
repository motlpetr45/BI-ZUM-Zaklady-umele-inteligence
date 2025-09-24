#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <random>
#include <cmath>
#include <iomanip>
#include <unordered_set>
#include <thread>
#include <fstream>
#include <algorithm>

using namespace std;

//-------------------------------------------------------------------------------------------------------------

struct IterationData {
    int conflicts;
    double temperature;
    unsigned long step;

};

//-------------------------------------------------------------------------------------------------------------

struct Coord {
    int m_Row;
    int m_Col;

    Coord(int row, int col) {
     m_Row = row;
     m_Col = col;
    }

    Coord() : m_Row(0), m_Col(0) {};
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Sudoku {
public:
    Sudoku(int gridSize);

    void setInitialValues(const vector<vector<int>> & initialValues);
    double getRandomDouble();

    void simulatedAnnealing();
    double computeProba(int fx, int fy, double t);

    void fillGrid();
    void print(unsigned long step, int conflicts, double temp);
    void logData(unsigned long step, int conflicts, double temp);
    void writeToFile(const string & path, int runNr);

    // Funkce pro počítání počtu konfliktů
    int calculateConflicts();
    int conflictsInLine(const vector<int>& line, unordered_set<int> &seen);
    int conflictsInBlock(int startRow, int startCol, int blockSize, unordered_set<int> &seen);

    // Počítání skóre tak, že za každý unikátní prvek v každém řádku a sloupci přičteme -1
    // v 9x9 máme 9 řádků a 9 sloupců = 18 -> každý -9 -> sudoku je vyřešeno, když se hodnota rovná -162
    int calculateScore(const vector<vector<int>>& grid);

    void revertSwappedCells(Coord & c1, Coord & c2);

    //*****Různé kandidátní funkce:*****

    // Prohození dvou náhodných buněk v náhodném řádku
    void swapCellsInRow();

    // Náhodně zvolíme subgrid a prohodíme dvě jeho náhodné buňky
    void swapCellsInSubGrid();

    // Náhodně zvolíme subgrid, v něm náhodně zvolíme řádek nebo sloupec a v něm prohodíme dvě náhodné buňky
    void swapCellsInSubGridRowCols();

private:
    int m_GridSize;
    int m_BlockSize;
    bool PRINT_MODE;
    mt19937 m_Gen;

    vector<vector<int>> m_Grid;
    vector<vector<bool>> m_Fixed;

    pair<Coord, Coord> m_Swapped;
    vector<IterationData> m_RunLog;
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

Sudoku::Sudoku(int gridSize) : m_GridSize(gridSize), m_BlockSize(sqrt(gridSize)), PRINT_MODE(false) {
    m_Grid.resize(m_GridSize);
    m_Fixed.resize(m_GridSize);

    for (int i = 0; i < m_GridSize; ++i) {
        m_Grid[i].resize(m_GridSize);
        m_Fixed[i].resize(m_GridSize);

        fill(m_Grid[i].begin(), m_Grid[i].end(), 0);
        fill(m_Fixed[i].begin(), m_Fixed[i].end(), 0);
    }

    auto seed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
    m_Gen.seed(seed);

}

//-------------------------------------------------------------------------------------------------------------

double Sudoku::getRandomDouble() {
    uniform_real_distribution<> distr(0.0, 1.0);

    return distr(m_Gen);
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::setInitialValues(const std::vector<std::vector<int>> &initialValues) {
    for(size_t i = 0; i < (size_t) m_GridSize; i++) {
        for (size_t j = 0; j < (size_t) m_GridSize; ++j) {
            m_Grid[i][j] = initialValues[i][j];
            if(m_Grid[i][j] != 0)
                m_Fixed[i][j] = true;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::fillGrid() {
    for (int blockNum = 0; blockNum < m_GridSize; blockNum++) {
        int blockRow = (blockNum / m_BlockSize) * m_BlockSize;
        int blockCol = (blockNum % m_BlockSize) * m_BlockSize;

        vector<pair<int, int>> zeroIndices;
        set<int> presentNumbers;

        for (int i = 0; i < m_BlockSize; i++) {
            for (int j = 0; j < m_BlockSize; j++) {
                int val = m_Grid[blockRow + i][blockCol + j];
                if (val != 0) {
                    presentNumbers.insert(val);
                } else {
                    zeroIndices.emplace_back(blockRow + i, blockCol + j);
                }
            }
        }

        vector<int> toFill;
        for (int num = 1; num <= m_GridSize; num++) {
            if (presentNumbers.find(num) == presentNumbers.end()) {
                toFill.push_back(num);
            }
        }

        shuffle(toFill.begin(), toFill.end(), m_Gen);

        for (size_t k = 0; k < zeroIndices.size(); k++) {
            auto& [r, c] = zeroIndices[k];
            m_Grid[r][c] = toFill[k];
        }
    }
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::print(unsigned long step, int conflicts, double temp) {
    string bold = "\033[1m";
    string reset = "\033[0m";

    cout << bold;
    cout << "STEP: " << step << "               " << endl;
    cout << "Score: " << conflicts << "               " << endl;
    cout << "Temperature: " << temp <<    "               " << endl;

    cout << reset;

    int largestNumber = floor(log10(m_GridSize)) + 1;
    string fullSeparator = "\033[1;31m+\033[0m";

    int totalLength = m_GridSize * (largestNumber + 1) + 2*m_BlockSize - 1;
    fullSeparator += string(totalLength, '-') + "\033[1;31m+\033[0m";

    for (int i = 0; i < m_GridSize; ++i) {
        if (i % m_BlockSize == 0)
            cout << fullSeparator << endl;

        for (int j = 0; j < m_GridSize; ++j) {
            if (j % m_BlockSize == 0)
                cout << "\033[1;31m| \033[0m";

            cout << "\033[34m" << setw(largestNumber) << m_Grid[i][j] << "\033[0m ";
        }
        cout << "\033[1;31m|\033[0m" << endl;
    }
    cout << fullSeparator << endl;
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::logData(unsigned long step, int conflicts, double temp) {
    m_RunLog.push_back({conflicts, temp, step});
}

//-------------------------------------------------------------------------------------------------------------

double Sudoku::computeProba(int fx, int fy, double t) {
    int delta = fy - fx;

    return exp(-delta / t);
}

//-------------------------------------------------------------------------------------------------------------

inline void Sudoku::swapCellsInRow() {
    uniform_int_distribution<> disRow(0, m_GridSize - 1);
    int row = disRow(m_Gen);

    // Collect all non-fixed column indices in the selected row
    vector<int> nonFixedCols;
    for (int col = 0; col < m_GridSize; col++) {
        if (!m_Fixed[row][col]) {
            nonFixedCols.push_back(col);
        }
    }

    if (nonFixedCols.size() > 1) {
        shuffle(nonFixedCols.begin(), nonFixedCols.end(), m_Gen);

        int col1 = nonFixedCols[0];
        int col2 = nonFixedCols[1];
        swap(m_Grid[row][col1], m_Grid[row][col2]);

        m_Swapped.first.m_Row = m_Swapped.second.m_Row = row;
        m_Swapped.first.m_Col = col1;
        m_Swapped.second.m_Col = col2;
    }
}

//-------------------------------------------------------------------------------------------------------------

inline int Sudoku::calculateConflicts() {
    int conflicts = 0;
    vector<int> column(m_GridSize);
    unordered_set<int> seen;

    for (int i = 0; i < m_GridSize; i++) {
        conflicts += conflictsInLine(m_Grid[i], seen);
        for (int j = 0; j < m_GridSize; j++) {
            column[j] = m_Grid[j][i];
        }
        conflicts += conflictsInLine(column, seen);
    }

    for (int blockRow = 0; blockRow < m_BlockSize; blockRow++) {
        for (int blockCol = 0; blockCol < m_BlockSize; blockCol++) {
            conflicts += conflictsInBlock(blockRow * m_BlockSize, blockCol * m_BlockSize, m_BlockSize, seen);
        }
    }

    return conflicts;
}

//-------------------------------------------------------------------------------------------------------------

inline int Sudoku::conflictsInLine(const vector<int> &line, unordered_set<int> &seen) {
    int conflict = 0;
    seen.clear();

    for (int num : line) {
        if (num != 0 && !seen.insert(num).second) {
            conflict++;
        }
    }
    return conflict;
}

//-------------------------------------------------------------------------------------------------------------

inline int Sudoku::conflictsInBlock(int startRow, int startCol, int blockSize, unordered_set<int> &seen) {
    seen.clear();
    int conflict = 0;
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            int num = m_Grid[startRow + i][startCol + j];
            if (num != 0 && !seen.insert(num).second) {
                conflict++;
            }
        }
    }
    return conflict;
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::revertSwappedCells(Coord &c1, Coord &c2) {
    swap(m_Grid[c1.m_Row][c1.m_Col], m_Grid[c2.m_Row][c2.m_Col]);
}

//-------------------------------------------------------------------------------------------------------------

int Sudoku::calculateScore(const vector<vector<int>> &grid) {
    int score = 0;
    unordered_set<int> elements;

    // Check rows
    for (const auto& row : grid) {
        elements.clear();
        elements.insert(row.begin(), row.end());
        score -= elements.size();
    }

    // Check columns
    for (size_t col = 0; col < grid.size(); ++col) {
        elements.clear();
        for (size_t row = 0; row < grid.size(); ++row) {
            elements.insert(grid[row][col]);
        }
        score -= elements.size();
    }

    return score;
}

//-------------------------------------------------------------------------------------------------------------

inline void Sudoku::swapCellsInSubGrid() {
    uniform_int_distribution<> disBlock(0, m_BlockSize - 1);

    int blockRow = disBlock(m_Gen) * m_BlockSize;
    int blockCol = disBlock(m_Gen) * m_BlockSize;

    vector<pair<int, int>> candidates;

    for (int i = 0; i < m_BlockSize; ++i) {
        for (int j = 0; j < m_BlockSize; ++j) {
            if (!m_Fixed[blockRow + i][blockCol + j]) {
                candidates.emplace_back(blockRow + i, blockCol + j);
            }
        }
    }

    if (candidates.size() > 1) {
        uniform_int_distribution<> dis(0, candidates.size() - 1);
        int first = dis(m_Gen);
        int second = dis(m_Gen);
        while (second == first) {
            second = dis(m_Gen);
        }
        swap(m_Grid[candidates[first].first][candidates[first].second],
             m_Grid[candidates[second].first][candidates[second].second]);

        m_Swapped.first.m_Row = candidates[first].first;
        m_Swapped.second.m_Row = candidates[second].first;
        m_Swapped.first.m_Col = candidates[first].second;
        m_Swapped.second.m_Col = candidates[second].second;
    }
}

//-------------------------------------------------------------------------------------------------------------

inline void Sudoku::swapCellsInSubGridRowCols() {
    uniform_int_distribution<> blockDist(0, m_BlockSize - 1);
    uniform_int_distribution<> flipCoin(0, 1);

    int blockRow = blockDist(m_Gen) * m_BlockSize;
    int blockCol = blockDist(m_Gen) * m_BlockSize;

    // Decide to swap within row or column
    if (flipCoin(m_Gen) == 0) {
        int row = blockRow + blockDist(m_Gen);
        int col1 = blockCol + blockDist(m_Gen);
        int col2 = (col1 + blockDist(m_Gen) % (m_BlockSize - 1) + 1) % m_BlockSize + blockCol;

        if (!m_Fixed[row][col1] && !m_Fixed[row][col2]) {
            swap(m_Grid[row][col1], m_Grid[row][col2]);
            m_Swapped.first.m_Row = m_Swapped.second.m_Row = row;
            m_Swapped.first.m_Col = col1;
            m_Swapped.second.m_Col = col2;
        }
    } else {
        int col = blockCol + blockDist(m_Gen);
        int row1 = blockRow + blockDist(m_Gen);
        int row2 = (row1 + blockDist(m_Gen) % (m_BlockSize - 1) + 1) % m_BlockSize + blockRow;

        if (!m_Fixed[row1][col] && !m_Fixed[row2][col]) {
            swap(m_Grid[row1][col], m_Grid[row2][col]);
            m_Swapped.first.m_Row = row1;
            m_Swapped.second.m_Row = row2;
            m_Swapped.first.m_Col = m_Swapped.second.m_Col = col;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::simulatedAnnealing() {
    double temp = 0.5;
    double coolingRate = 0.99999;

    // Iterations without improvement before termination
    int stagnationLimit = 5000;

    // Track last improvement
    int lastImprovement = 0;

    fillGrid();
    int conflicts = calculateScore(m_Grid);
    unsigned long step = 0;

    cout << "START SCORE: " << conflicts << endl;

    while (lastImprovement < stagnationLimit) {
        swapCellsInSubGrid();

        int newConflicts = calculateScore(m_Grid);

        // Found solution
        if(newConflicts == (m_GridSize*2*m_GridSize))
            break;

        if(newConflicts >= conflicts) {
            lastImprovement++;
        } else {
            lastImprovement = 0;
        }

        if((newConflicts > conflicts) && (computeProba(conflicts, newConflicts, temp) < getRandomDouble()) ) {
            revertSwappedCells(m_Swapped.first, m_Swapped.second);
        } else {
            conflicts = newConflicts;
        }

        temp = temp * coolingRate;
        step++;

        if(PRINT_MODE) {
            cout << "\033[H";
            print(step, conflicts, temp);
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        logData(step, conflicts, temp);
    }

    if(!PRINT_MODE) {
        print(step, conflicts, temp);
    }
}

//-------------------------------------------------------------------------------------------------------------

void Sudoku::writeToFile(const string & path, int runNr) {
    ofstream out(path, ios::out | ios::app);

    for (size_t i = 0; i < m_RunLog.size(); ++i) {
        out << runNr << "," << m_RunLog[i].step << "," << m_RunLog[i].conflicts << endl;
    }

    m_RunLog.clear();
    out.close();
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

int main(int argc, char ** argv) {
    vector<vector<int>> sudokuInit1 = {
            {0, 5, 0,    0, 7, 0,    0, 0, 0},
            {2, 1, 3,    0, 0, 0,    0, 0, 0},
            {9, 8, 0,    4, 0, 1,    0, 0, 2},

            {7, 0, 0,    0, 0, 0,    0, 0, 1},
            {0, 9, 0,    7, 0, 5,    0, 3, 0},
            {4, 0, 0,    0, 0, 0,    0, 0, 5},

            {5, 0, 0,    9, 0, 4,    0, 2, 6},
            {0, 0, 0,    0, 0, 0,    1, 8, 7},
            {0, 0, 0,    0, 8, 0,    0, 4, 0}
    };

    Sudoku sudoku1{9};
    sudoku1.setInitialValues(sudokuInit1);
    // sudoku1.simulatedAnnealing();

//-------------------------------------------------------------------------------------------------------------

    vector<vector<int>> sudokuInit2 = {
            {5, 3, 0,    0, 7, 0,    0, 0, 0},
            {6, 0, 0,    1, 9, 5,    0, 0, 0},
            {0, 9, 8,    0, 0, 0,    0, 6, 0},

            {8, 0, 0,    0, 6, 0,    0, 0, 3},
            {4, 0, 0,    8, 0, 3,    0, 0, 1},
            {7, 0, 0,    0, 2, 0,    0, 0, 6},

            {0, 6, 0,    0, 0, 0,    2, 8, 0},
            {0, 0, 0,    4, 1, 9,    0, 0, 5},
            {0, 0, 0,    0, 8, 0,    0, 7, 9}
    };
/*    Sudoku sudoku2{9};

    string path = "./log3.csv";

    ofstream out(path, ios::out | ios::app);
    out << "run,iteration,score" << endl;
    out.close();

    for (int i = 0; i < 10; ++i) {
        sudoku2.setInitialValues(sudokuInit2);
        sudoku2.simulatedAnnealing();
        sudoku2.writeToFile(path, i+1);
    }*/


//-------------------------------------------------------------------------------------------------------------

    vector<vector<int>> sudokuInit3 = {
            { 0, 12, 0, 0,     0, 14, 0, 6,     0, 10, 9, 0,     16, 0, 0, 11 },
            { 1, 5, 0, 13,     16, 10, 12, 0,   4, 15, 0, 6,     8, 0, 0, 0 },
            { 0, 0, 4, 3,      15, 8, 0, 11,    0, 1, 13, 0,     6, 0, 0, 9 },
            { 9, 7, 10, 0,     0, 2, 0, 3,      5, 8, 11, 16,    14, 13, 15, 0 },

            { 16, 13, 0, 0,    10, 3, 2, 7,     0, 0, 8, 11,     5, 0, 1, 0 },
            { 10, 4, 3, 0,     0, 0, 0, 12,     1, 0, 0, 0,      0, 0, 7, 6 },
            { 7, 11, 6, 0,     0, 0, 0, 0,      3, 2, 4, 12,     13, 8, 10, 15 },
            { 0, 2, 0, 0,      13, 1, 6, 0,     10, 0, 15, 9,    3, 11, 0, 14 },

            { 6, 0, 15, 11,    2, 5, 0, 14,     0, 12, 10, 7,    0, 0, 8, 0 },
            { 3, 8, 7, 12,     11, 4, 9, 13,    0, 0, 0, 0,      0, 2, 14, 5 },
            { 13, 9, 0, 0,     0, 0, 0, 1,      2, 0, 0, 0,      0, 3, 12, 7 },
            { 0, 14, 0, 4,     7, 12, 0, 0,     8, 11, 3, 13,    0, 0, 6, 1 },

            { 0, 6, 1, 9,     12, 13, 4, 2,     11, 0, 7, 0,     0, 14, 3, 16 },
            { 4, 0, 0, 2,     0, 6, 14, 0,      16, 0, 12, 8,    7, 5, 0, 0 },
            { 0, 0, 0, 7,     9, 0, 11, 8,      0, 3, 6, 14,     1, 0, 13, 2 },
            { 11, 0, 0, 16,   0, 7, 10, 0,      13, 0, 2, 0,     0, 0, 9, 0 }
    };

    Sudoku sudoku3{16};
    sudoku3.setInitialValues(sudokuInit3);
    // sudoku3.simulatedAnnealing();

/*    string path = "./log6.csv";

    ofstream out(path, ios::out | ios::app);
    out << "run,iteration,score" << endl;
    out.close();

    for (int i = 0; i < 10; ++i) {
        sudoku3.setInitialValues(sudokuInit3);
        sudoku3.simulatedAnnealing();
        sudoku3.writeToFile(path, i+1);
    }*/

//-------------------------------------------------------------------------------------------------------------

    vector<vector<int>> sudokuInit4 = {
            { 8, 7, 0, 0,      0, 0, 0, 0,      0, 3, 0, 0,      13, 0, 4, 0 },
            { 0, 5, 14, 0,     0, 0, 3, 10,     15, 9, 1, 0,     0, 6, 0, 0 },
            { 16, 0, 0, 0,     5, 8, 7, 0,      0, 14, 0, 0,     9, 0, 11, 12 },
            { 0, 0, 4, 0,      0, 14, 6, 13,    0, 11, 10, 12,   0, 7, 0, 3 },

            { 14, 0, 0, 8,     0, 0, 1, 0,      0, 0, 0, 3,      7, 4, 12, 0 },
            { 9, 0, 0, 0,      0, 6, 15, 12,    0, 0, 13, 14,    0, 3, 1, 0 },
            { 11, 0, 10, 3,    0, 0, 13, 0,     0, 8, 0, 1,      0, 0, 6, 0 },
            { 6, 0, 0, 1,      14, 0, 4, 0,     0, 5, 0, 9,      11, 0, 0, 13 },

            { 0, 0, 0, 0,      15, 0, 0, 0,     0, 0, 9, 0,      5, 0, 2, 10 },
            { 10, 1, 0, 0,     6, 0, 5, 0,      13, 15, 7, 16,   0, 0, 0, 0 },
            { 0, 0, 16, 11,    0, 4, 0, 8,      2, 0, 0, 0,      0, 13, 0, 7 },
            { 0, 9, 0, 7,      1, 3, 0, 2,      6, 0, 8, 10,     16, 15, 14, 4 },

            { 7, 0, 13, 0,     9, 16, 0, 5,     0, 0, 14, 4,     3, 8, 0, 2 },
            { 0, 0, 3, 0,      10, 0, 0, 0,     0, 0, 0, 0,      0, 16, 15, 0 },
            { 1, 0, 9, 0,      0, 0, 14, 4,     0, 0, 0, 0,      0, 0, 7, 0 },
            { 0, 6, 8, 0,      3, 0, 0, 0,      10, 7, 0, 0,     0, 0, 0, 0 }
    };

    Sudoku sudoku4{16};
    sudoku4.setInitialValues(sudokuInit4);
    // sudoku4.simulatedAnnealing();
    // sudoku2.print(0,0,0);

//-------------------------------------------------------------------------------------------------------------
    Sudoku sudoku5{25};
    // sudoku5.simulatedAnnealing();

/*    string path = "./log7.csv";

    ofstream out(path, ios::out | ios::app);
    out << "run,iteration,score" << endl;
    out.close();

    for (int i = 0; i < 10; ++i) {
        Sudoku sudoku5{25};
        sudoku5.simulatedAnnealing();
        sudoku5.writeToFile(path, i+1);
    }*/
}