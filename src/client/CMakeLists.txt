cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE CLIENT_SOURCES
    "*.cpp"
    "*.hpp"
    "*/*.cpp"
    "*/*.hpp"
)

add_executable(r-type_client ${CLIENT_SOURCES})

if(WIN32)
    set(SFML_ROOT "C:/SFML-2.5.1/lib/cmake/SFML")
endif()

target_include_directories(r-type_client PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Find SFML
find_package(SFML 2.5.1 COMPONENTS system window graphics audio)

if(NOT SFML_FOUND)
    # External dependencies with version control
    CPMAddPackage(
        NAME SFML
        GITHUB_REPOSITORY SFML/SFML
        GIT_TAG 2.5.x
        OPTIONS
            "SFML_BUILD_AUDIO ON"
            "SFML_BUILD_GRAPHICS ON"
            "SFML_BUILD_WINDOW ON"
            "SFML_BUILD_NETWORK OFF"
            "SFML_BUILD_EXAMPLES OFF"
            "SFML_BUILD_DOC OFF"
            "SFML_BUILD_TEST OFF"
    )
endif()

target_link_libraries(r-type_client PRIVATE 
common
    sfml-graphics 
    sfml-window 
    sfml-audio
    ASIO::ASIO
    ${PLATFORM_LIBS}
)

if(UNIX)
    target_compile_options(r-type_client PRIVATE -Wall -Wextra -Werror)
elseif(WIN32)
    target_compile_definitions(r-type_client PRIVATE _WIN32_WINNT=0x0A00)
endif()

if(WIN32)
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_DIR}/../../../bin" $<TARGET_FILE_DIR:r-type_client>)
endif()

# Install rules
install(TARGETS r-type_client DESTINATION bin)
install(DIRECTORY assets DESTINATION share/r-type/client)