function(add_engine_interface_module MODULE_NAME)
    file(GLOB_RECURSE MODULE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/include/${MODULE_NAME}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/include/${MODULE_NAME}/*.hpp"
    )

    add_library(${MODULE_NAME} INTERFACE)

    target_sources(${MODULE_NAME} INTERFACE
        ${MODULE_HEADERS}
    )

    target_include_directories(${MODULE_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Modules/${MODULE_NAME}/include>
        $<INSTALL_INTERFACE:include>
    )
endfunction()