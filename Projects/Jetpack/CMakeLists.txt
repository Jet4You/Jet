cmake_minimum_required(VERSION 3.28)

project(JetpackApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES YES)

file(GLOB_RECURSE PRIVATE_MODULE_SOURCES
  "Private/*.cppm"
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PRIVATE
  FILE_SET cxx_modules_private TYPE CXX_MODULES FILES
  ${PRIVATE_MODULE_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "jetp")
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  # Jet

  # Third party
  Jet_Comp_Foundation
  Jet_Comp_Format
  Jet_Comp_YAML
)

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif ()