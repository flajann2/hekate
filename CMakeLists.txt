cmake_minimum_required(VERSION 3.10)

# Project name and version
project(hekate  LANGUAGES CXX)

option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_EXAMPLES "Enable examples" OFF)
option(ENABLE_DOCS "Enable documentation" OFF)

include(CTest)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

set(INCLUDE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

message(STATUS "hetake include ${INCLUDE_PATH}")
  
# Define GNU standard installation directories
include(GNUInstallDirs)

# Add feature summary
include(FeatureSummary)

# C++ compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)


add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

if (ENABLE_DOCS)
  add_subdirectory(docs)
endif()

if (ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if (ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
