cmake_minimum_required(VERSION 3.18.2)

set(CMAKE_CXX_STANDARD 20)

project(Comfy VERSION 0.0.1)

include(FetchContent)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        release-1.11.0
)

message(STATUS "Populating googletest")
FetchContent_MakeAvailable(googletest)
message(STATUS "Populating googletest - done")

enable_testing()

add_subdirectory(src)