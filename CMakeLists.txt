cmake_minimum_required(VERSION 4.0)
project(
	dim-lang	
	VERSION 0.1.0
	LANGUAGES C CXX
)

# General C++ Version flags

set(CMAKE_CXX_STANDARD 		23)
set(CMAKE_CXX_STANDARD_REQUIRED	YES)
set(CMAKE_CXX_EXTENSIONS	OFF)

# Binary directory

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files

set(NR_SRC
	${PROJECT_SOURCE_DIR}/src/main.cxx

	${PROJECT_SOURCE_DIR}/src/utils/utils.cxx
	
	${PROJECT_SOURCE_DIR}/src/lexer/lexer.cxx
	
	${PROJECT_SOURCE_DIR}/src/parser/ast.cxx
	${PROJECT_SOURCE_DIR}/src/parser/cast.cxx
	${PROJECT_SOURCE_DIR}/src/parser/parser.cxx

	${PROJECT_SOURCE_DIR}/src/runner/interpreter/value.cxx
	${PROJECT_SOURCE_DIR}/src/runner/interpreter/interpreter.cxx
	${PROJECT_SOURCE_DIR}/src/runner/interpreter/register.cxx
)

# Result executable

add_executable(${PROJECT_NAME} ${NR_SRC})

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	quadmath
)

set(CMAKE_BUILD_TYPE Debug)

