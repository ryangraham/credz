cmake_minimum_required(VERSION 3.10)
project(credz VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FIND_PACKAGE(Boost COMPONENTS filesystem program_options regex REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
find_package(AWSSDK REQUIRED COMPONENTS sts)

IF(APPLE)
	FIND_LIBRARY(SECURITY Security)
ENDIF(APPLE)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

add_executable(credz src/main.cc src/aws.cc src/okta.cc src/cli.cc src/xml.cc src/keychain.cc src/ini.cc)

TARGET_LINK_LIBRARIES(credz LINK_PUBLIC ${Boost_LIBRARIES} curl ${AWSSDK_LINK_LIBRARIES} ${SECURITY})

add_definitions(-DPATCH_VERSION="")

ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/cmake/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version.h
                     -D PROJECT_VERSION=${CMAKE_PROJECT_VERSION}
                     -D PATCH_VERSION=${PATCH_VERSION}
                     -P ${CMAKE_SOURCE_DIR}/cmake/version.cmake
)
ADD_DEPENDENCIES(credz version)

install(TARGETS credz DESTINATION bin)
