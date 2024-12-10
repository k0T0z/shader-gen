#################################################################################
#                                                                               #
#  Copyright (C) 2024 Seif Kandil (k0T0z)                                       #
#                                                                               #
#  This file is a part of the ENIGMA Development Environment.                   #
#                                                                               #
#                                                                               #
#  ENIGMA is free software: you can redistribute it and/or modify it under the  #
#  terms of the GNU General Public License as published by the Free Software    #
#  Foundation, version 3 of the license or any later version.                   #
#                                                                               #
#  This application and its source code is distributed AS-IS, WITHOUT ANY       #
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    #
#  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        #
#  details.                                                                     #
#                                                                               #
#  You should have recieved a copy of the GNU General Public License along      #
#  with this code. If not, see <http://www.gnu.org/licenses/>                   #
#                                                                               #
#  ENIGMA is an environment designed to create games and other programs with a  #
#  high-level, fully compilable language. Developers of ENIGMA or anything      #
#  associated with ENIGMA are in no way responsible for its users or            #
#  applications created by its users, or damages caused by the environment      #
#  or programs made in the environment.                                         #
#                                                                               #
#################################################################################

cmake_minimum_required(VERSION 3.15)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Build type not set - defaulting to Debug")
  set(
    CMAKE_BUILD_TYPE "Debug"
    CACHE
      STRING
      "Choose the type of build from:  Debug Release MinSizeRel RelWithDebInfo."
    FORCE)
endif()

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_MINSIZEREL_POSTFIX "s")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")

set(ENIGMA_ORG_NAME "ENIGMA Development Environment")
set(SHADER_GEN_PROJECT_NAME "Shader Gen")
set(SHADER_GEN_PROJECT_VERSION "1.0.0")
set(SHADER_GEN_PROJECT_DESCRIPTION "Shader Gen is a tool to generate shaders using machine learning.")
set(SHADER_GEN_EXECUTABLE_NAME "shader-gen")

project(${SHADER_GEN_EXECUTABLE_NAME} VERSION ${SHADER_GEN_PROJECT_VERSION} LANGUAGES CXX)

if (WIN32)
    string(APPEND CMAKE_CXX_FLAGS " /W4")
else()
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")
    string(APPEND CMAKE_C_FLAGS " -Wall -Wextra")
endif()

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n"
  )
endif()

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Create code from a list of Qt designer ui files.
# set(CMAKE_AUTOUIC ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Create code from Qt rc files.
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable compiler-specific extensions for cross-platform compatibility.
set(CMAKE_CXX_EXTENSIONS OFF)

set(SHADER_GEN_ROOTDIR ${PROJECT_SOURCE_DIR})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    # Define a macro for Debug builds
    set(SHADER_GEN_DEBUG_MACRO_NAME "SHADER_GEN_DEBUG")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(-fsanitize=address -g -O1)
        add_link_options(-fsanitize=address)
    else()
        message(WARNING "ASan is not supported by this compiler.")
    endif()
endif()

if(MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /EHsc /wd26812")
    string(APPEND CMAKE_C_FLAGS " /EHsc /wd26812")
endif()

# Disable C++ exceptions.
if(MSVC)
    string(REGEX REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_definitions(-D_HAS_EXCEPTIONS=0)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables")
endif()

# Check https://stackoverflow.com/q/33062728/14629018 for more information.
if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

set(SHADER_GEN_QT_VERSION "5")

file(GLOB SHADER_GEN_MAIN_ENTRY 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

file(GLOB SHADER_GEN_HPP_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader_generator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader_node_generators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/vs_node_noise_generators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/proto_model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/message_model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/repeated_message_model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/model_mapper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/primitive_model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/repeated_primitive_model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/oneof_model.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/controller/visual_shader_editor.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/utils/field_path.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/utils/utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/error_macros.hpp
)

file(GLOB SHADER_GEN_CPP_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader_generator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader_node_generators.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/vs_node_noise_generators.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/proto_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/message_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/repeated_message_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/model_mapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/primitive_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/repeated_primitive_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/oneof_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/controller/visual_shader_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/utils/field_path.cpp
)

file(GLOB SHADER_GEN_PROTO_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/schema/visual_shader.proto
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/schema/visual_shader_nodes.proto
)

file(GLOB SHADER_GEN_UI_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/view/visual_shader_editor.ui
)

if (WIN32)
    add_executable(${SHADER_GEN_EXECUTABLE_NAME} WIN32
        ${SHADER_GEN_MAIN_ENTRY} 
        ${SHADER_GEN_CPP_FILES} 
        ${SHADER_GEN_PROTO_GENERATED_SRCS} # Include the generated protobuf files
    )
else()
    add_executable(${SHADER_GEN_EXECUTABLE_NAME}  
        ${SHADER_GEN_MAIN_ENTRY} 
        ${SHADER_GEN_CPP_FILES} 
        ${SHADER_GEN_PROTO_GENERATED_SRCS} # Include the generated protobuf files
    )
endif()

# OpenGLWidgets not required for Qt5 LTS
find_package(Qt${SHADER_GEN_QT_VERSION} CONFIG REQUIRED COMPONENTS Widgets Test)

# Find Protobuf
find_package(Protobuf CONFIG REQUIRED)

# Create the output directory for the generated protobuf files
set(PROTOC_OUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gui/model/schema")
file(MAKE_DIRECTORY ${PROTOC_OUT_DIRECTORY})

# https://stackoverflow.com/a/54254098/14629018
protobuf_generate(
    TARGET ${SHADER_GEN_EXECUTABLE_NAME}
    LANGUAGE cpp
    OUT_VAR SHADER_GEN_PROTO_GENERATED_SRCS
    PROTOC_OUT_DIR ${PROTOC_OUT_DIRECTORY} # Output directory for the generated files
    PROTOS ${SHADER_GEN_PROTO_FILES} # The proto files
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gui/model/schema # For the imports in the proto files
)

target_link_libraries(${SHADER_GEN_EXECUTABLE_NAME} PRIVATE 
    Qt${SHADER_GEN_QT_VERSION}::Widgets 
    protobuf::libprotobuf
)

target_include_directories(${SHADER_GEN_EXECUTABLE_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/" 
    "${CMAKE_CURRENT_BINARY_DIR}/" # For the generated protobuf files
)

target_compile_definitions(${SHADER_GEN_EXECUTABLE_NAME} PRIVATE 
    ENIGMA_ORG_NAME="${ENIGMA_ORG_NAME}"
    SHADER_GEN_PROJECT_NAME="${SHADER_GEN_PROJECT_NAME}"
    SHADER_GEN_PROJECT_VERSION="${SHADER_GEN_PROJECT_VERSION}"
    $<$<CONFIG:Debug>:${SHADER_GEN_DEBUG_MACRO_NAME}> # Define ${SHADER_GEN_DEBUG_MACRO_NAME} for Debug builds
)

#####################
# Tests
#####################

set(SHADER_GEN_TESTS_EXECUTABLE_NAME "shader-gen-tests" CACHE STRING "The name of the tests executable.")

file(GLOB SHADER_GEN_TESTS_CPP_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/gui/model/test_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/gui/model/utils/test_field_path.cpp
)

file(GLOB SHADER_GEN_TESTS_PROTO_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/gui/model/schema/test_schema.proto
)

# For Windows: Prevent overriding the parent project's compiler/linker settings as mentioned in the official documentation.
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# Check https://cmake.org/cmake/help/latest/module/FindGTest.html for more information.
find_package(GTest CONFIG)
if(NOT GTest_FOUND)
    set(SHADER_GEN_TESTS_GTEST_MAIN_HEADER "${SHADER_GEN_ROOTDIR}/externals/googletest/googletest/include/gtest/gtest.h")

    if (NOT EXISTS "${SHADER_GEN_TESTS_GTEST_MAIN_HEADER}")
        message(FATAL_ERROR "Cannot find GTest package.")
    endif()

    message(STATUS "Found GTest: ${SHADER_GEN_TESTS_GTEST_MAIN_HEADER}")

    option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." ON)
    option(BUILD_GMOCK "Builds the googlemock subproject" OFF)
    option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)
    option(
        gtest_force_shared_crt
        "Use shared (DLL) run-time lib even when Google Test is built as static lib."
        ON)
    
    set(SHADER_GEN_TESTS_GTEST_ROOTDIR "${SHADER_GEN_ROOTDIR}/externals/googletest")
    set(SHADER_GEN_TESTS_GTEST_BUILD_ROOTDIR "${PROJECT_BINARY_DIR}/.googletest-build")
    add_subdirectory(${SHADER_GEN_TESTS_GTEST_ROOTDIR} ${SHADER_GEN_TESTS_GTEST_BUILD_ROOTDIR})
endif()

if (WIN32)
    add_executable(${SHADER_GEN_TESTS_EXECUTABLE_NAME} WIN32
        ${SHADER_GEN_TESTS_CPP_FILES} 
        ${SHADER_GEN_CPP_FILES} 
        ${SHADER_GEN_PROTO_GENERATED_SRCS} # Include the generated protobuf files
        ${SHADER_GEN_PROTO_GENERATED_TEST_SRCS} # Include the generated protobuf test files
    )
else()
    add_executable(${SHADER_GEN_TESTS_EXECUTABLE_NAME} 
        ${SHADER_GEN_TESTS_CPP_FILES} 
        ${SHADER_GEN_CPP_FILES} 
        ${SHADER_GEN_PROTO_GENERATED_SRCS} # Include the generated protobuf files
        ${SHADER_GEN_PROTO_GENERATED_TEST_SRCS} # Include the generated protobuf test files
    )
endif()

# Create the output directory for the generated protobuf test files
set(PROTOC_TEST_OUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests/gui/model/schema")
file(MAKE_DIRECTORY ${PROTOC_TEST_OUT_DIRECTORY})

protobuf_generate(
    TARGET ${SHADER_GEN_TESTS_EXECUTABLE_NAME}
    LANGUAGE cpp
    OUT_VAR SHADER_GEN_PROTO_GENERATED_TEST_SRCS
    PROTOC_OUT_DIR ${PROTOC_TEST_OUT_DIRECTORY} # Output directory for the generated test files
    PROTOS ${SHADER_GEN_TESTS_PROTO_FILES} # The proto test files
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/tests/gui/model/schema # For the imports in the proto test files
)

target_link_libraries(${SHADER_GEN_TESTS_EXECUTABLE_NAME} PRIVATE 
    GTest::gtest 
    GTest::gtest_main 
    Qt${SHADER_GEN_QT_VERSION}::Widgets 
    Qt${SHADER_GEN_QT_VERSION}::Test
    protobuf::libprotobuf 
)

target_include_directories(${SHADER_GEN_TESTS_EXECUTABLE_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${SHADER_GEN_ROOTDIR}/externals/googletest/googletest/include" 
    "${CMAKE_CURRENT_BINARY_DIR}/" # For the generated protobuf test files
)

target_compile_definitions(${SHADER_GEN_TESTS_EXECUTABLE_NAME} PRIVATE 
    $<$<CONFIG:Debug>:${SHADER_GEN_DEBUG_MACRO_NAME}> # Define ${SHADER_GEN_DEBUG_MACRO_NAME} for Debug builds
)
