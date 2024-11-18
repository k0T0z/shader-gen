#################################################################################
#                                                                               #
#  Copyright (C) 2024 Saif Kandil (k0T0z)                                       #
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

# TODO: We can use an older version of CMake.
cmake_minimum_required(VERSION 3.5)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader_nodes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/vs_noise_nodes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/visual_shader_editor.hpp
)

file(GLOB SHADER_GEN_CPP_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/visual_shader_nodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generator/vs_noise_nodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/visual_shader_editor.cpp
)

# Safety check: Ensure that all the files exist.
foreach(file ${SHADER_GEN_CPP_FILES} ${SHADER_GEN_HPP_FILES} ${SHADER_GEN_MAIN_ENTRY})
    if(NOT EXISTS "${file}")
        message(FATAL_ERROR "File '${file}' does not exist!")
    endif()
endforeach()

set(${SHADER_GEN_EXECUTABLE_NAME}_INCLUDE_DIRECTORIES 
    "${CMAKE_CURRENT_SOURCE_DIR}/" 
)

find_package(Qt${SHADER_GEN_QT_VERSION} CONFIG REQUIRED COMPONENTS Widgets)

add_executable(${SHADER_GEN_EXECUTABLE_NAME}  
    ${SHADER_GEN_MAIN_ENTRY} 
    ${SHADER_GEN_CPP_FILES} 
)

target_link_libraries(${SHADER_GEN_EXECUTABLE_NAME} PRIVATE 
    Qt${SHADER_GEN_QT_VERSION}::Widgets 
)

target_include_directories(${SHADER_GEN_EXECUTABLE_NAME} PRIVATE 
    ${${SHADER_GEN_EXECUTABLE_NAME}_INCLUDE_DIRECTORIES} 
)

target_compile_definitions(${SHADER_GEN_EXECUTABLE_NAME} PRIVATE 
    ENIGMA_ORG_NAME="${ENIGMA_ORG_NAME}"
    SHADER_GEN_PROJECT_NAME="${SHADER_GEN_PROJECT_NAME}"
    SHADER_GEN_PROJECT_VERSION="${SHADER_GEN_PROJECT_VERSION}"
)
