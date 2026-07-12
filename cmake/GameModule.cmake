function(add_game_module MODULE_NAME)
    file(GLOB_RECURSE MODULE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/include/${MODULE_NAME}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/include/${MODULE_NAME}/*.hpp"
    )

    add_library(${MODULE_NAME} ${MODULE_SOURCES})

    target_include_directories(${MODULE_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/include
    )
endfunction()