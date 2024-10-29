#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

// Наивный алгоритм поиска подстроки
int substring_search(const std::string& text, const std::string& pattern) {
    int occurrences = 0;
    size_t text_len = text.length();
    size_t pattern_len = pattern.length();

    for (size_t i = 0; i <= text_len - pattern_len; ++i) {
        if (text.substr(i, pattern_len) == pattern) {
            occurrences++;
        }
    }
    return occurrences;
}

void search_in_file(const std::string& filename, const std::string& pattern, int repetitions) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file." << std::endl;
        return;
    }

    // Чтение содержимого файла
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    int total_occurrences = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Повторение поиска
    for (int i = 0; i < repetitions; ++i) {
        total_occurrences += substring_search(text, pattern);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    std::cout << "Substring found " << total_occurrences << " times." << std::endl;
    std::cout << "execution time: " << elapsed_time.count() << " sec." << std::endl;
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
