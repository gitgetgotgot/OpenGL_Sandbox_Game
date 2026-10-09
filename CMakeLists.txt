cmake_minimum_required(VERSION 3.30)
project(OpenGL_2D_Sandbox VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/GameModule.cmake)

add_game_module(Audio)
add_game_module(Core)
add_game_module(Entities)
add_game_module(Objects)
add_game_module(Physics)
add_game_module(Rendering)
add_game_module(Resources)
add_game_module(UI)
add_game_module(IOSystem)
add_game_module(Utility)
add_game_module(EngineAPI)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(stb REQUIRED)
find_package(miniaudio REQUIRED)
find_package(glad REQUIRED)

add_executable(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/src/main.cpp")

target_link_libraries(Rendering
    PUBLIC
        glad::glad
        glfw glm::glm
        stb::stb
)

target_link_libraries(Utility
    PUBLIC
        glm::glm
        nlohmann_json::nlohmann_json
    PRIVATE
        stb::stb
)

target_link_libraries(Resources PUBLIC Utility)

target_link_libraries(IOSystem
    PRIVATE
        glad::glad
        glfw
)
target_link_libraries(IOSystem PUBLIC Utility)

target_link_libraries(Audio PUBLIC miniaudio::miniaudio)

target_link_libraries(Physics PUBLIC glm::glm)

target_link_libraries(UI PUBLIC Rendering)
target_link_libraries(UI PUBLIC Resources)
target_link_libraries(UI PUBLIC IOSystem)
target_link_libraries(UI PUBLIC Audio)

target_link_libraries(Objects PUBLIC UI)
target_link_libraries(Objects PUBLIC Physics)

target_link_libraries(Entities PUBLIC Objects)

target_link_libraries(Core PUBLIC Entities)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)

target_link_libraries(EngineAPI PUBLIC Entities)


target_compile_definitions(Core PRIVATE
    $<$<CONFIG:Debug>:RESOURCE_ROOT="${PROJECT_SOURCE_DIR}">
    $<$<CONFIG:Release>:RESOURCE_ROOT="">
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:DEBUG_MODE=0>
)

add_executable(DataCompiler
    "${PROJECT_SOURCE_DIR}/Tools/DataCompiler/DataCompilerMain.cpp"
)
target_link_libraries(DataCompiler PRIVATE Utility)

#add_custom_command(
#    TARGET ${PROJECT_NAME}
#    PRE_BUILD
#    COMMAND DataCompiler
#        ${PROJECT_SOURCE_DIR}/Resources/game_data
#        ${PROJECT_SOURCE_DIR}/Resources/bin_data
#    COMMENT "Compiling core JSON to binary DATA..."
#)
