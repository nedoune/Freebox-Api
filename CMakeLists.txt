cmake_minimum_required(VERSION 3.0.0)
project(freeboxapi VERSION 0.1.0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
include_directories(/home/ned/Downloads/boost_1_74_0)
link_directories(/home/ned/Downloads/boost_1_74_0/stage/lib/)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(freeboxapi ${SOURCES})
target_link_libraries(freeboxapi boost_program_options-mt-x64)
target_link_libraries(freeboxapi ${CURL_LIBRARIES})
target_link_libraries(freeboxapi ${OPENSSL_LIBRARIES})



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
