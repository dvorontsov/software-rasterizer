cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Software-Renderer C)

file(GLOB_RECURSE SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/*.c)

file(GLOB_RECURSE HEADER_FILES
		${CMAKE_SOURCE_DIR}/src/*.h)
		
		
# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
)

configure_file(src/utils/root_dir.h.in src/utils/root_dir.h)

include(CheckLibraryExists)
message("Check if math lib exists and needs to be linked")
set(MATH_LIB m)
CHECK_LIBRARY_EXISTS(${MATH_LIB} sqrt "" MATH_LIB_EXISTS)

#Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
include_directories(${CMAKE_BINARY_DIR}/src)

#Linking with math library
if(${MATH_LIB_EXISTS})
	target_link_libraries(${PROJECT_NAME} ${MATH_LIB})
endif(${MATH_LIB_EXISTS})

