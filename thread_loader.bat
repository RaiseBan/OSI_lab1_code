@echo off
:: Запрашиваем количество экземпляров для каждой программы
set /p num="Enter number of instances for each thread loader: "

:: Запуск указанного количества экземпляров
echo run %num% instances binary_search.exe...
for /l %%i in (1,1,%num%) do (
    start "" C:\Users\user\CLionProjects\ALGO\combined_loader.exe D:\large_text.txt R7 10000
)

echo good