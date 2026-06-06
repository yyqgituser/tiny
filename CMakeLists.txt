cmake_minimum_required(VERSION 3.20)
project(tiny VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

if(NOT CMAKE_PREFIX_PATH)
    if(WIN32)
        list(APPEND CMAKE_PREFIX_PATH
            "$ENV{USERPROFILE}/.csio-dev"
            "$ENV{USERPROFILE}/llvm/x64-windows"
        )
    endif()
endif()
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release "")
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release "")

find_package(csio REQUIRED)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} at ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(LLVM_LIBS
    core support object mc nativecodegen
    x86codegen x86asmparser x86desc x86info
    Analysis TransformUtils InstCombine ScalarOpts Passes
)

file(GLOB GENERATED_SOURCES generated-src/*.cpp)
file(GLOB AST_SOURCES src/ast/*.cpp)
file(GLOB BUILDER_SOURCES src/builder/*.cpp)
file(GLOB SEMA_SOURCES src/sema/*.cpp)
file(GLOB OPTIMIZE_SOURCES src/optimize/*.cpp)

add_executable(tiny
    src/main/tiny.cpp
    src/codegen/TargetContext.cpp
    src/codegen/CodeGen.cpp
    src/linker/Linker.cpp
    src/loader/Loader.cpp
    ${GENERATED_SOURCES}
    ${AST_SOURCES}
    ${BUILDER_SOURCES}
    ${SEMA_SOURCES}
    ${OPTIMIZE_SOURCES}
)
target_include_directories(tiny PUBLIC src generated-src)
target_link_libraries(tiny PRIVATE csio::csio ${LLVM_LIBS})
