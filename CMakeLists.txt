cmake_minimum_required(VERSION 3.15)
project(R-Type 
    VERSION 1.0
    DESCRIPTION "A GAME ENGINE THAT ROARS !"
    LANGUAGES CXX
)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom Find modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Dependencies management
include(cmake/CPM.cmake)

# Find required packages
find_package(ASIO REQUIRED)

# Add subdirectories
add_subdirectory(src/common) 
add_subdirectory(src/engine)
add_subdirectory(src/client)
add_subdirectory(src/server)

# Platform-specific configurations
if(UNIX)
    set(PLATFORM_LIBS X11 GL)
elseif(WIN32)
    set(PLATFORM_LIBS opengl32)
endif()