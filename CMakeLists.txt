cmake_minimum_required(VERSION 3.18)
project(image_loader_lib LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(imageloader main.cpp ome_tiff_loader.cpp)

# PyBind
find_package(Python COMPONENTS Interpreter Development)
add_subdirectory(lib/pybind11)

if (Python_FOUND OR Python_INCLUDE_DIRS)
	add_compile_definitions(WITH_PYTHON_H)
endif()


include_directories(
	lib/pybind11/include/
	${Python_INCLUDE_DIRS}
)

message(STATUS "PYTHON HEADERS: ${Python_INCLUDE_DIRS}")

pybind11_add_module(image_loader_lib ome_tiff_loader.cpp image_loader_interface.cpp)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-modules")

find_package(FastLoader REQUIRED)
if(FastLoader_FOUND)
	list(APPEND image_loader_LIBRARIES ${FastLoader_LIBRARIES})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FastLoader_CXX_FLAGS}")
	include_directories(${FastLoader_INCLUDE_DIR})
else()
	message(FATAL_ERROR "Unable to find FastLoader which is required for Image Loader.")
endif()

target_link_libraries(image_loader_lib PRIVATE stdc++fs)
target_link_libraries(image_loader_lib PRIVATE ${image_loader_LIBRARIES})

target_link_libraries(imageloader PRIVATE stdc++fs)
target_link_libraries(imageloader PRIVATE ${image_loader_LIBRARIES})

#target_link_libraries(TARGET ${FastLoader_LIBRARIES} ${Hedgehog_LIBRARIES})
#target_include_directories(TARGET PUBLIC ${FastLoader_INCLUDE_DIR} ${Hedgehog_INCLUDE_DIR})