cmake_minimum_required(VERSION 3.11)
project(credz VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.7.3
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

SET(BOOST_MIN_VERSION "1.71.0")
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

FetchContent_Declare(
  cfg
  GIT_REPOSITORY https://github.com/ryangraham/cfg.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(cfg)
include_directories(${cfg_SOURCE_DIR}/include)

FetchContent_Declare(
  keyring
  GIT_REPOSITORY https://github.com/ryangraham/keyring.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(keyring)
include_directories(${keyring_SOURCE_DIR}/include)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
find_package(AWSSDK REQUIRED COMPONENTS sts)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

INCLUDE_DIRECTORIES(thirdparty)

add_executable(credz src/main.cc src/aws.cc src/okta.cc src/cli.cc src/xml.cc src/ini.cc)

TARGET_LINK_LIBRARIES(credz LINK_PUBLIC ${Boost_LIBRARIES} curl ${AWSSDK_LINK_LIBRARIES} cfg keyring nlohmann_json::nlohmann_json)

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
