#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

// Геномы клеток
struct Genome {
    float reproduction_speed; // Скорость размножения
    float resistance;        // Устойчивость к неблагоприятным факторам
    float immunity;          // Иммунитет против других клеток
};

using Row = std::vector<std::pair<bool, Genome>>;
using Cells = std::vector<Row>;

// Начальные размеры игрового поля
int numRows = 40;
int numCols = 120;


#define COLOR_RESET "\033[0m"
#define CELL_COLOR_RED "\033[38;5;196m" // Красные клетки
#define CELL_COLOR_GREEN "\033[38;5;46m" // Зелёные клетки
#define CELL_COLOR_BLUE "\033[38;5;21m"  // Голубые клетки
#define CELL_COLOR_YELLOW "\033[38;5;226m" // Жёлтые клетки
#define CELL_COLOR_PURPLE "\033[38;5;99m"  // Фиолетовые клетки


Genome initial_genome = { 1.0f, 1.0f, 1.0f };


float gene_mutation_range = 0.1f;


Genome copyWithMutation(const Genome& g) {
    Genome mutated_gene = g;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-gene_mutation_range, gene_mutation_range);

    mutated_gene.reproduction_speed *= (1.f + distribution(generator));
    mutated_gene.resistance *= (1.f + distribution(generator));
    mutated_gene.immunity *= (1.f + distribution(generator));

    return mutated_gene;
}

// Получаем значение соседней клетки
int getNeighbor(int row, int col, const Cells& board) {
    return board.at((row + numRows) % numRows).at((col + numCols) % numCols).first;
}

// Подсчет количества живых соседей
int getCount(int row, int col, const Cells& board) {
    int count = 0;
    std::vector<int> deltas{ -1, 0, 1 };
    for (int dc : deltas) {
        for (int dr : deltas) {
            if ((dr || dc)) {
                int neighbor_type = getNeighbor(row + dr, col + dc, board);
                if (neighbor_type) {
                    count++;
                }
            }
        }
    }
    return count;
}

// Вывод клетки на экран с учётом её генома
void showCell(bool alive, const Genome& genome) {
    if (alive) {
        float avg = (genome.reproduction_speed + genome.resistance + genome.immunity) / 3.0f;
        if (avg < 0.8f) {
            std::cout << CELL_COLOR_RED << "@" << COLOR_RESET; // Красные клетки
        }
        else if (avg < 1.2f) {
            std::cout << CELL_COLOR_GREEN << "@" << COLOR_RESET; // Зелёные клетки
        }
        else if (avg < 1.6f) {
            std::cout << CELL_COLOR_BLUE << "@" << COLOR_RESET; // Голубые клетки
        }
        else if (avg < 2.0f) {
            std::cout << CELL_COLOR_YELLOW << "@" << COLOR_RESET; // Жёлтые клетки
        }
        else {
            std::cout << CELL_COLOR_PURPLE << "@" << COLOR_RESET; // Фиолетовые клетки
        }
    }
    else {
        std::cout << ".";
    }
}


void showRow(const Row& row) {
    std::cout << "| ";
    for (const auto& cell : row) {
        showCell(cell.first, cell.second);
    }
    std::cout << " |" << std::endl;
}

// Вывод всего поля
void showCells(const Cells& board) {
    std::cout << "+-" << std::string(numCols, '-') << "+" << std::endl;
    for (const auto& row : board) {
        showRow(row);
    }
    std::cout << "+-" << std::string(numCols, '-') << "+" << std::endl;
}

// Логика обновления клетки
int tick(const Cells& board, int row, int col) {
    int current_alive = board[row][col].first;
    int count = getCount(row, col, board);

    if (current_alive) {
        bool survive = count == 2 || count == 3;
        return survive ? 1 : 0;
    }
    else {
        bool birth = count == 3;
        return birth ? 1 : 0;
    }
}


void updateCells(Cells& board) {
    Cells original = board;
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            int next_state = tick(original, row, col);
            if (next_state == 1) {
                // Наследуем геном
                if (original[row][col].first) {
                    board[row][col].second = original[row][col].second;
                }
                else {
                    // Найдём ближайший подходящий источник генома
                    int best_parent_row = row;
                    int best_parent_col = col;
                    float max_fitness = 0.0f;
                    std::vector<int> deltas{ -1, 0, 1 };
                    for (int dc : deltas) {
                        for (int dr : deltas) {
                            if ((dr || dc)) {
                                int nr = (row + dr + numRows) % numRows;
                                int nc = (col + dc + numCols) % numCols;
                                if (original[nr][nc].first) {
                                    float fitness = original[nr][nc].second.reproduction_speed +
                                        original[nr][nc].second.resistance +
                                        original[nr][nc].second.immunity;
                                    if (fitness > max_fitness) {
                                        max_fitness = fitness;
                                        best_parent_row = nr;
                                        best_parent_col = nc;
                                    }
                                }
                            }
                        }
                    }
                    board[row][col].second = copyWithMutation(original[best_parent_row][best_parent_col].second);
                }
            }
            else {
                board[row][col].second = {}; // Обнуляем геном мёртвой клетки
            }
            board[row][col].first = next_state;
        }
    }
}


int main() {
    srand((time(0)));
    double i = 0;

    Cells board(numRows, Row(numCols, { false, {} }));
    std::default_random_engine generator;
    std::bernoulli_distribution dist(0.2);

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            if (dist(generator)) {
                board[row][col].first = true;
                board[row][col].second = initial_genome;
            }
        }
    }

    while (true) {
        i++;
        system("cls");
        std::cout << "\ngeneration: " << i << std::endl;
        showCells(board);
        updateCells(board);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
