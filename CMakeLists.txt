cmake_minimum_required(VERSION 3.12)
project(aws-credz VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(Boost COMPONENTS program_options regex REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
find_package(AWSSDK REQUIRED COMPONENTS sts)

add_executable(aws-credz src/main.cpp)

TARGET_LINK_LIBRARIES(aws-credz LINK_PUBLIC ${Boost_LIBRARIES} curl ${AWSSDK_LINK_LIBRARIES})
