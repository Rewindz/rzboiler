cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(boiler)

include(FetchContent)

FetchContent_Declare(rzutils
  GIT_REPOSITORY "https://github.com/Rewindz/rzutils"
  GIT_TAG main
  GIT_SHALLOW ON
)

FetchContent_Declare(cli11
  GIT_REPOSITORY "https://github.com/CLIUtils/CLI11"
  GIT_TAG "v2.6.2"
  GIT_SHALLOW ON
)

FetchContent_MakeAvailable(rzutils cli11)

set(BIN_SRC
  "main.cpp"
)

list(TRANSFORM BIN_SRC PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/src/")

add_executable(boiler)
target_sources(boiler PRIVATE ${BIN_SRC})
target_link_libraries(boiler PRIVATE rzutils CLI11::CLI11)
