cmake_minimum_required(VERSION 3.18)
#set(CMAKE_VERBOSE_MAKEFILE ON)
project(image_loader_lib LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(imageloader main.cpp ome_tiff_loader.cpp)
#add_executable(imageloader main_fastloader_example.cpp)

#OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	message(STATUS "OpenMP Found: ${OpenMP_CXX_FLAGS}")
else()
	message(STATUS "OpenMP Not Found")
endif()

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

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)


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
message(STATUS ${CMAKE_CXX_FLAGS})

target_link_libraries(image_loader_lib PRIVATE stdc++fs)
target_link_libraries(image_loader_lib PRIVATE ${image_loader_LIBRARIES})
#target_link_libraries(image_loader_lib PRIVATE Threads::Threads)
target_link_libraries(imageloader PRIVATE stdc++fs)
target_link_libraries(imageloader PRIVATE ${image_loader_LIBRARIES})
#target_link_libraries(image_loader_lib PRIVATE Threads::Threads)
#target_link_libraries(TARGET ${FastLoader_LIBRARIES} ${Hedgehog_LIBRARIES})
#target_include_directories(TARGET PUBLIC ${FastLoader_INCLUDE_DIR} ${Hedgehog_INCLUDE_DIR})