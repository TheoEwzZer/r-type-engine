# Collect source files
file(GLOB COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

# Create the library
add_library(common STATIC ${COMMON_SOURCES})

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(common PUBLIC ASIO::ASIO)

if(WIN32)
    target_link_libraries(common PUBLIC ws2_32)
endif()

find_package(ZLIB)
if(NOT ZLIB_FOUND)
    CPMAddPackage(
        NAME ZLIB
        GITHUB_REPOSITORY madler/zlib
        VERSION 1.3.1
    )
    if(TARGET zlibstatic)
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
    elseif(TARGET zlib)
        add_library(ZLIB::ZLIB ALIAS zlib)
    endif()
endif()

target_link_libraries(common PUBLIC ZLIB::ZLIB)