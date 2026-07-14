cmake_minimum_required(VERSION 3.30)
project(OpenGL_2D_Sandbox VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

include(cmake/GameModule.cmake)
include(cmake/ExternalLibs.cmake)

add_game_module(Audio)
add_game_module(Core)
add_game_module(Entities)
add_game_module(Objects)
add_game_module(Physics)
add_game_module(Rendering)
add_game_module(UI)
add_game_module(IOSystem)
add_game_module(Utility)

file(GLOB_RECURSE MAIN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

target_link_libraries(Rendering PUBLIC ExternalLibs)
# add system OpenGL library to Rendering module
if (WIN32)
    target_link_libraries(Rendering PUBLIC opengl32)
elseif (APPLE)
    target_link_libraries(Rendering PUBLIC "-framework OpenGL")
else() # Linux and other
    find_package(OpenGL REQUIRED)
    target_link_libraries(Rendering PUBLIC OpenGL::GL)
endif()
# add glad.c to Rendering module
target_sources(Rendering
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Rendering/src/glad.c
)

target_link_libraries(Utility PUBLIC ExternalLibs)

target_link_libraries(IOSystem PUBLIC ExternalLibs)
target_link_libraries(IOSystem PUBLIC Utility)

target_link_libraries(Audio PUBLIC ExternalLibs)

target_link_libraries(Physics PUBLIC ExternalLibs)

target_link_libraries(Objects PUBLIC Rendering)
target_link_libraries(Objects PUBLIC IOSystem)
target_link_libraries(Objects PUBLIC Audio)
target_link_libraries(Objects PUBLIC Physics)

target_link_libraries(Entities PUBLIC Objects)

target_link_libraries(UI PUBLIC Entities)

target_link_libraries(Core PUBLIC UI)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)

set(GAME_DATA_DIRS
    Resources
    Saves
)

foreach(dir ${GAME_DATA_DIRS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/${dir}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dir}
    )
endforeach()