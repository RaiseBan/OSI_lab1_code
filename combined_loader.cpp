#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>  // для Windows API

// Функция для наивного поиска подстроки
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

struct SubstringSearchParams {
    std::string filename;
    std::string pattern;
    int repetitions;
    int* total_occurrences;
};

DWORD WINAPI search_in_file(LPVOID param) {
    auto* args = (SubstringSearchParams*)param;
    std::ifstream file(args->filename);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file." << std::endl;
        return 1;
    }

    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    for (int i = 0; i < args->repetitions; ++i) {
        *(args->total_occurrences) += substring_search(text, args->pattern);
    }
    return 0;
}

struct BinarySearchParams {
    int array_size;
    int target;
    int repetitions;
    int* found_count;
};

DWORD WINAPI perform_binary_search(LPVOID param) {
    auto* args = (BinarySearchParams*)param;
    std::vector<int> sorted_array(args->array_size);
    for (int i = 0; i < args->array_size; ++i) {
        sorted_array[i] = i;
    }

    for (int i = 0; i < args->repetitions; ++i) {
        int left = 0;
        int right = sorted_array.size() - 1;
        bool found = false;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (sorted_array[mid] == args->target) {
                found = true;
                break;
            }
            if (sorted_array[mid] < args->target)
                left = mid + 1;
            else
                right = mid - 1;
        }
        if (found) {
            (*(args->found_count))++;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <substr> <array_size>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string pattern = argv[2];
    int repetitions = 5;

    int array_size = std::stoi(argv[3]);
    int target = array_size / 2;
    int binary_repetitions = 1000;

    int substring_occurrences = 0;
    int binary_found_count = 0;

    SubstringSearchParams substring_params = {filename, pattern, repetitions, &substring_occurrences};
    BinarySearchParams binary_params = {array_size, target, binary_repetitions, &binary_found_count};

    // Создание потоков
    HANDLE substring_thread = CreateThread(nullptr, 0, search_in_file, &substring_params, 0, nullptr);
    HANDLE binary_thread = CreateThread(nullptr, 0, perform_binary_search, &binary_params, 0, nullptr);

    // Ожидание завершения потоков
    WaitForSingleObject(substring_thread, INFINITE);
    WaitForSingleObject(binary_thread, INFINITE);

    // Закрытие потоков
    CloseHandle(substring_thread);
    CloseHandle(binary_thread);

    std::cout << "Substring occurrences: " << substring_occurrences << std::endl;
    std::cout << "Binary search found count: " << binary_found_count << std::endl;

    return 0;
}
