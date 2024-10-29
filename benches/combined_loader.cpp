#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <windows.h>

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

// Функция для поиска подстроки в строке с использованием алгоритма КМП
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

struct SubstringSearchParams {
    std::string filename;
    std::string pattern;
    int repetitions;
    int* total_occurrences;
    double* execution_time; // Добавлено для хранения времени выполнения
};

DWORD WINAPI search_in_file(LPVOID param) {
    auto* args = (SubstringSearchParams*)param;

    // Начало измерения времени
    auto start_time = std::chrono::high_resolution_clock::now();

    std::ifstream file(args->filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file." << std::endl;
        return 1;
    }

    const size_t BUFFER_SIZE = 1024 * 1024; // Размер буфера (1 МБ)
    size_t pattern_length = args->pattern.length();

    // Вычисляем префикс-функцию для образца
    std::vector<int> pi = computePrefixFunction(args->pattern);

    int total_occurrences = 0;

    // Повторение поиска
    for (int i = 0; i < args->repetitions; ++i) {
        file.clear(); // Сбрасываем флаги EOF
        file.seekg(0, std::ios::beg); // Перемещаемся в начало файла

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
            KMPMatcher(text, args->pattern, pi, total_occurrences);

            // Сохраняем перекрытие для следующего блока
            if (bytes_read >= pattern_length - 1) {
                previous_overlap = text.substr(text.length() - (pattern_length - 1));
            } else {
                previous_overlap = text;
            }
        }
    }

    file.close();

    // Конец измерения времени
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    *(args->execution_time) = elapsed.count(); // Сохраняем время выполнения

    // Обновляем общее количество найденных подстрок
    *(args->total_occurrences) = total_occurrences;

    return 0;
}

struct BinarySearchParams {
    int array_size;
    int target;
    int repetitions;
    int* found_count;
    double* execution_time; // Добавлено для хранения времени выполнения
};

DWORD WINAPI perform_binary_search(LPVOID param) {
    auto* args = (BinarySearchParams*)param;

    // Начало измерения времени
    auto start_time = std::chrono::high_resolution_clock::now();

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

    // Конец измерения времени
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    *(args->execution_time) = elapsed.count(); // Сохраняем время выполнения

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <substr> <array_size>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string pattern = argv[2];
    int repetitions = 10;

    int array_size = std::stoi(argv[3]);
    int target = 1; // Устанавливаем target на 1
    int binary_repetitions = 250000000;  // Устанавливаем 25,000,000 повторений для binary_search

    int substring_occurrences = 0;
    int binary_found_count = 0;

    double substring_execution_time = 0.0;
    double binary_execution_time = 0.0;

    SubstringSearchParams substring_params = {
            filename,
            pattern,
            repetitions,
            &substring_occurrences,
            &substring_execution_time // Передаем указатель на переменную для времени выполнения
    };

    BinarySearchParams binary_params = {
            array_size,
            target,
            binary_repetitions,
            &binary_found_count,
            &binary_execution_time // Передаем указатель на переменную для времени выполнения
    };

    // Создание потоков
    HANDLE substring_thread = CreateThread(nullptr, 0, search_in_file, &substring_params, 0, nullptr);
    HANDLE binary_thread = CreateThread(nullptr, 0, perform_binary_search, &binary_params, 0, nullptr);

    if (substring_thread == nullptr || binary_thread == nullptr) {
        std::cerr << "Error: Failed to create threads." << std::endl;
        return 1;
    }

    // Ожидание завершения потоков
    WaitForSingleObject(substring_thread, INFINITE);
    WaitForSingleObject(binary_thread, INFINITE);

    // Закрытие потоков
    CloseHandle(substring_thread);
    CloseHandle(binary_thread);

    std::cout << "Substring occurrences: " << substring_occurrences << std::endl;
    std::cout << "Substring search execution time: " << substring_execution_time << " seconds" << std::endl;

    std::cout << "Binary search found count: " << binary_found_count << std::endl;
    std::cout << "Binary search execution time: " << binary_execution_time << " seconds" << std::endl;

    return 0;
}
