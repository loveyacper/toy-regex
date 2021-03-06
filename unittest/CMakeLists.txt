#PROJECT(UNITTEST)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(CMAKE_CXX_FLAGS_DEBUG  "-g -Wall -std=c++0x")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -Wall -std=c++0x")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS_DEBUG  "-g -Wall -std=c++11 -stdlib=libc++ -Wc++11-extensions")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -Wall -std=c++11 -stdlib=libc++ -Wc++11-extensions")
ELSE()
    #    message(FATAL_ERROR "Only support linux or OS X, support for windows is in plan")
ENDIF()

OPTION(DEBUG "Debug or release" OFF)

IF(DEBUG)
    SET(CMAKE_BUILD_TYPE "Debug")
ELSE()
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

AUX_SOURCE_DIRECTORY(. UNITTEST_SRC)
AUX_SOURCE_DIRECTORY(../ UNITTEST_SRC)

INCLUDE_DIRECTORIES(../)

ADD_EXECUTABLE(toy_regex_test ${UNITTEST_SRC})
SET(EXECUTABLE_OUTPUT_PATH  ../)
