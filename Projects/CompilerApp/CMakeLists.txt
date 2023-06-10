cmake_minimum_required(VERSION 3.25)

project(RigCCompilerApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES
  "Public/*.hpp"
  "Public/*.ixx"
  "Public/*.cpp"
  "Private/*.hpp"
  "Private/*.ixx"
  "Private/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "rigcc")
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  # RigC
    RigCCompiler RigCParser RigCCore
  # Third party
    RigC_Comp_Format
)

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif ()