#include <windows.h>
#include <iostream>
#include <string>
#include <chrono>

bool CreateProcessWithCommand(const std::string& command) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Команда с аргументами передается как есть
    char cmdLine[1024];
    strcpy_s(cmdLine, command.c_str());

    // Получение времени старта процесса
    auto start_time = std::chrono::high_resolution_clock::now();

    // Запуск процесса с использованием CreateProcessAsUserA
    if (!CreateProcessAsUser(
            nullptr,               // Дескриптор пользователя
            nullptr,               // Имя приложения
            cmdLine,               // Командная строка (включая аргументы)
            nullptr,               // Атрибуты безопасности процесса
            nullptr,               // Атрибуты безопасности потока
            FALSE,                 // Унаследование дескрипторов
            0,                     // Флаги создания
            nullptr,               // Переменные среды
            nullptr,               // Текущая директория
            &si,                   // Стартовая информация
            &pi                    // Информация о процессе
    )) {
        std::cerr << "CreateProcessAsUserA failed (" << GetLastError() << ").\n";
        return false;
    }

    // Ожидание завершения процесса
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Получение времени завершения процесса
    auto end_time = std::chrono::high_resolution_clock::now();

    // Вычисление времени выполнения
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Program execution time: " << elapsed.count() << " sec\n";

    // Закрытие дескрипторов процесса
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

void displayHelp() {
    std::cout << "\navailable commands:\n";
    std::cout << "  exit  - terminate shell\n";
    std::cout << "  help  - commands info\n";
    std::cout << "  <command>  - execute command with args (exp.:, C:\\path\\to\\program.exe args)\n";
}

int main() {
    std::cout << "You can find more info here: <<help>>\n";
    while (true) {
        std::string command;
        std::cout << "enter command: \n";
        std::getline(std::cin, command);

        // Проверка на команду выхода
        if (command == "exit") {
            std::cout << "terminate shell.\n";
            break;
        }

        // Проверка на команду помощи
        if (command == "help") {
            displayHelp();
            continue;
        }

        // Выполнение команды
        if (!CreateProcessWithCommand(command)) {
            std::cerr << "err.\n";
        }
    }

    return 0;
}
