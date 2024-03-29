cmake_minimum_required(VERSION 3.28)

project(JetCompiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES YES)

file(GLOB_RECURSE PUBLIC_MODULE_SOURCES
  "Public/*.cppm"
  "Public/*.ixx"
)

file(GLOB_RECURSE PRIVATE_MODULE_SOURCES
  "Private/*.cppm"
  "Private/*.ixx"
)

file(GLOB_RECURSE PRIVATE_SOURCES
  "Private/*.cpp"
)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
  PUBLIC
    FILE_SET CXX_MODULES TYPE CXX_MODULES FILES
    ${PUBLIC_MODULE_SOURCES}
  PRIVATE
    FILE_SET cxx_modules_private TYPE CXX_MODULES FILES
    ${PRIVATE_MODULE_SOURCES}
  PRIVATE
    ${PRIVATE_SOURCES} 
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
      JetParser JetCore
    PUBLIC
      Jet_Comp_Format
)

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif ()