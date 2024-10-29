#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

// Бинарный поиск элемента в отсортированном массиве
bool binary_search(const std::vector<int>& sorted_array, int target) {
    int left = 0;
    int right = sorted_array.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (sorted_array[mid] == target)
            return true; // Элемент найден
        if (sorted_array[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;


    }
    return false; // Элемент не найден
}

void perform_binary_search(const std::vector<int>& sorted_array, int target, long long repetitions) {
    auto start_time = std::chrono::high_resolution_clock::now();

    for (long long i = 0; i < repetitions; ++i) {
        binary_search(sorted_array, target);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    std::cout << "execution time of binary_search " << elapsed_time.count() << " sec\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " <array capacity> <target value> <repeat count>\n";
        return 1;
    }

    // Парсим аргументы командной строки
    long array_size = std::stol(argv[1]);
    int target = std::stoi(argv[2]);
    long long repetitions = std::stoll(argv[3]); // Изменено на stoll для больших значений

    // Генерация отсортированного массива целых чисел
    std::vector<int> sorted_array(array_size);
    for (long i = 0; i < array_size; ++i) {
        sorted_array[i] = i; // Заполняем массив числами от 0 до array_size - 1
    }

    // Выполнение бинарного поиска
    perform_binary_search(sorted_array, target, repetitions);

    return 0;
}
