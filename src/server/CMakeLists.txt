cmake_minimum_required(VERSION 3.14)

file(GLOB_RECURSE SERVER_SOURCES
    "*.cpp"
    "*.hpp"
)

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)

add_executable(r-type_server ${SERVER_SOURCES})

target_include_directories(r-type_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(ASIO REQUIRED)

target_link_libraries(r-type_server PRIVATE 
    common
    engine
    ASIO::ASIO
    nlohmann_json::nlohmann_json
)

if(UNIX)
    target_compile_options(r-type_server PRIVATE -Wall -Wextra -Werror)
elseif(WIN32)
    target_compile_definitions(r-type_server PRIVATE _WIN32_WINNT=0x0A00)
endif()

# Install rules
install(TARGETS r-type_server DESTINATION bin)