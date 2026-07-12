add_library(ExternalLibs INTERFACE)

target_include_directories(ExternalLibs INTERFACE
    ${CMAKE_SOURCE_DIR}/Libraries/include
)

target_link_directories(ExternalLibs INTERFACE
    ${CMAKE_SOURCE_DIR}/Libraries/lib
)

target_link_libraries(ExternalLibs INTERFACE
    glfw3
    SDL2
    SDL2main
    SDL2test
    SDL2_mixer
)
