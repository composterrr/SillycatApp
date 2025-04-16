cmake_minimum_required(VERSION 3.15)
project(SillycatApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Указываем путь к заголовочным файлам и библиотекам 
include_directories("${CMAKE_SOURCE_DIR}/libs/crow/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/asio")
include_directories("${CMAKE_SOURCE_DIR}/libs/postgres17/include")
link_directories("${CMAKE_SOURCE_DIR}/libs/postgres17/lib")

include_directories("${CMAKE_SOURCE_DIR}/libs")

# Добавляем исполняемый файл
add_executable(SillycatApp SillycatApp.cpp)

# Линкуем с libpq
target_link_libraries(SillycatApp libpq)
