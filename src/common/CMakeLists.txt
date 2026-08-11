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

find_package(ZLIB REQUIRED)
target_link_libraries(common PUBLIC ZLIB::ZLIB)