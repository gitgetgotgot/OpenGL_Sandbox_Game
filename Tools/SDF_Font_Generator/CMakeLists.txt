cmake_minimum_required(VERSION 3.30)
project(SDF_Font_Generator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(stb REQUIRED)
find_package(freetype REQUIRED)

add_executable(${PROJECT_NAME}
	"${PROJECT_SOURCE_DIR}/src/generator_main.cpp"
	"${PROJECT_SOURCE_DIR}/src/FontGenerator.cpp"
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		stb::stb
		Freetype::Freetype
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/InputFonts"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/InputFonts"
    COMMENT "Copy Resources folder in build folder"
)