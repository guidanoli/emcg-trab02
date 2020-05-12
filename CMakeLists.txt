cmake_minimum_required(VERSION 3.0)
project(seega)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL)
find_package(GLUT)

if (OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIRS})
endif()

if (GLUT_FOUND)
	include_directories(${GLUT_INCLUDE_DIRS})
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_subdirectory("src")
add_subdirectory("include")
add_subdirectory("tests")

if (OPENGL_FOUND AND GLUT_FOUND)
	add_subdirectory("vis")
endif()