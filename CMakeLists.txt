cmake_minimum_required(VERSION 3.26)
project(ALGO)

set(CMAKE_CXX_STANDARD 17)

# Добавляем исполняемый файл
add_executable(ALGO main.cpp)

## Указываем манифест для запроса прав администратора
#set_target_properties(ALGO PROPERTIES
#        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
#)
#
## Добавляем манифест в проект
#target_sources(ALGO PRIVATE app.manifest)
