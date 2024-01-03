cmake_minimum_required(VERSION 3.28)

project(JetTestProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES YES)

file(GLOB_RECURSE PRIVATE_SOURCES
  "Private/*.hpp"
  "Private/*.cpp"
)

file(GLOB_RECURSE PRIVATE_MODULE_SOURCES
  "Private/*.cppm"
  "Private/*.ixx"
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PRIVATE
    ${PRIVATE_SOURCES}
  PRIVATE
    FILE_SET cxx_modules_private TYPE CXX_MODULES FILES
    ${PRIVATE_MODULE_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE JetCompiler JetParser JetCore Jet_Comp_Format gtest gtest_main)

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif ()