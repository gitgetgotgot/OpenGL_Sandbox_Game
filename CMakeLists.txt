cmake_minimum_required(VERSION 3.30)
project(OpenGL_2D_Sandbox VERSION 1.0)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported configurations" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/EngineModule.cmake)
include(cmake/EngineInterfaceModule.cmake)

add_engine_interface_module(Base)
add_engine_module(Audio)
add_engine_module(Core)
add_engine_module(Entities)
add_engine_module(Objects)
add_engine_module(Physics)
add_engine_module(Rendering)
add_engine_module(Resources)
add_engine_module(UI)
add_engine_module(DeviceSystem)
add_engine_module(Utility)
add_engine_module(EngineAPI)

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
        Base
)

target_link_libraries(Utility
    PUBLIC
        glm::glm
        nlohmann_json::nlohmann_json
        Base
    PRIVATE
        stb::stb
)

target_link_libraries(Resources
    PUBLIC
        Utility
        Base
    PRIVATE
        stb::stb
        Rendering
)

target_link_libraries(DeviceSystem
    PUBLIC
        Base
    PRIVATE
        glad::glad
        glfw
)
target_link_libraries(DeviceSystem PUBLIC Utility)

target_link_libraries(Audio PUBLIC miniaudio::miniaudio)
target_link_libraries(Audio PRIVATE Utility)

target_link_libraries(Physics PUBLIC glm::glm)
target_link_libraries(Physics PUBLIC Base)

target_link_libraries(UI PUBLIC Rendering)
target_link_libraries(UI PUBLIC Resources)
target_link_libraries(UI PUBLIC DeviceSystem)
target_link_libraries(UI PUBLIC Audio)

target_link_libraries(Objects PUBLIC UI)
target_link_libraries(Objects PUBLIC Physics)

target_link_libraries(Entities PUBLIC Objects)

target_link_libraries(Core PUBLIC Entities)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)

target_link_libraries(EngineAPI PUBLIC Entities)


target_compile_definitions(Utility PUBLIC
    $<$<CONFIG:Debug>:CMAKE_DEBUG_MACRO=1>
)
if(MSVC)
    target_compile_options(Rendering PRIVATE
        /W4                 # (Max Warning Level 4)
        /permissive-        # turn off incompatible Microsoft extensions (strict C++ standard)
        /Zc:__cplusplus     # correct macros __cplusplus (C++23)
        /Zc:preprocessor    # modern preprocessing
    )
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
    COMMAND ${CMAKE_COMMAND} -E 
        $<IF:$<CONFIG:Debug>,create_symlink,copy_directory>
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
    COMMAND ${CMAKE_COMMAND} -E echo "Resources successfully configured for $<CONFIG> mode."
    COMMENT "Configuring Resources (Symlink for Debug, Copy for Release)."
)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Config"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Config"
    COMMENT "Copy Config folder to build folder."
)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Mods"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Mods"
    COMMENT "Copy Config folder to build folder."
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
