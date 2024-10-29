#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

// Размер буфера чтения (например, 1 МБ)
const size_t BUFFER_SIZE = 1024 * 1024;

// Функция для вычисления префикс-функции (алгоритм КМП)
std::vector<int> computePrefixFunction(const std::string& pattern) {
    size_t m = pattern.length();
    std::vector<int> pi(m, 0);
    int k = 0;
    for (size_t q = 1; q < m; ++q) {
        while (k > 0 && pattern[k] != pattern[q])
            k = pi[k - 1];
        if (pattern[k] == pattern[q])
            ++k;
        pi[q] = k;
    }
    return pi;
}

// Функция для поиска подстроки в строке с использованием префикс-функции
void KMPMatcher(const std::string& text, const std::string& pattern, const std::vector<int>& pi, int& occurrences) {
    size_t n = text.length();
    size_t m = pattern.length();
    int q = 0;
    for (size_t i = 0; i < n; ++i) {
        while (q > 0 && pattern[q] != text[i])
            q = pi[q - 1];
        if (pattern[q] == text[i])
            ++q;
        if (q == m) {
            occurrences++;
            q = pi[q - 1];
        }
    }
}

void search_in_file(const std::string& filename, const std::string& pattern, int repetitions) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file." << std::endl;
        return;
    }

    // Вычисляем префикс-функцию для образца
    std::vector<int> pi = computePrefixFunction(pattern);
    size_t pattern_length = pattern.length();

    int total_occurrences = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Повторение поиска
    for (int rep = 0; rep < repetitions; ++rep) {
        file.clear(); // Сбрасываем флаги потока
        file.seekg(0, std::ios::beg); // Перемещаемся в начало файла

        size_t total_bytes_read = 0;
        std::string previous_overlap;

        while (!file.eof()) {
            // Читаем блок из файла
            std::vector<char> buffer(BUFFER_SIZE);
            file.read(buffer.data(), BUFFER_SIZE);
            size_t bytes_read = file.gcount();

            if (bytes_read == 0)
                break;

            std::string text;

            // Добавляем перекрытие с предыдущим блоком
            if (!previous_overlap.empty()) {
                text = previous_overlap + std::string(buffer.data(), bytes_read);
            } else {
                text = std::string(buffer.data(), bytes_read);
            }

            // Ищем подстроку в текущем блоке
            KMPMatcher(text, pattern, pi, total_occurrences);

            // Сохраняем перекрытие для следующего блока
            if (bytes_read >= pattern_length - 1) {
                previous_overlap = text.substr(text.length() - (pattern_length - 1));
            } else {
                previous_overlap = text;
            }

            total_bytes_read += bytes_read;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    file.close();

    std::cout << "Substring found " << total_occurrences << " times." << std::endl;
    std::cout << "Execution time: " << elapsed_time.count() << " sec." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " <filename> <substr> <number of repetitions>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string pattern = argv[2];
    int repetitions = std::stoi(argv[3]);

    search_in_file(filename, pattern, repetitions);

    return 0;
}
