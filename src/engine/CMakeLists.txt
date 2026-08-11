add_library(engine STATIC
    GameEngine.cpp
    GameEngineInit.cpp
    AssetManager.cpp
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/common/src)
target_link_libraries(engine PUBLIC common)
