# SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
#
# SPDX-License-Identifier: MIT-0

add_compile_options(-Wall -Wextra -Wpedantic -Werror -pedantic-errors -Wsign-conversion -g)

cmake_minimum_required(VERSION 3.26)
project(Concordo)

include_directories(include)

# The binary will be placed into ./bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

# Create compile_commands.json on ./build for usage with clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS=ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_executable(concordo
              src/main.cpp
              src/channels.cpp
              src/system.cpp
              src/users.cpp
              src/servers.cpp)
