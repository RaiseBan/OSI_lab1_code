@echo off
:: Запрашиваем количество экземпляров для каждой программы
set /p num="Enter number of instances for each loader: "

:: Запуск указанного количества экземпляров substring_search.exe
echo run %num% instances substring_search.exe...
for /l %%i in (1,1,%num%) do (
    start "" C:\Users\user\CLionProjects\ALGO\substring_search D:\large_text.txt R7 2
)

:: Запуск указанного количества экземпляров binary_search.exe
echo run %num% instances binary_search.exe...
for /l %%i in (1,1,%num%) do (
    start "" C:\Users\user\CLionProjects\ALGO\binary_search.exe 100000 1 250000000
)

echo good
