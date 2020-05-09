cmake_minimum_required(VERSION 3.0.0)
project(cassandra_integration VERSION 0.1.0)

set(ALT_PATH "/usr/local")
find_path(
    CASS_H cassandra.h
    PATHS ${ALT_PATH}/include
)
find_library(
    CASS_LIB cassandra 
    PATHS ${ALT_PATH}/bin
)

if(CASS_H)
    message("Cassandra header file found at - ${CASS_H}")
endif()

if(CASS_LIB)
    message("Cassandra library executable found at - ${CASS_LIB}")
endif()

add_executable(cassandra_integration check_cass_version.cc)
target_include_directories(cassandra_integration PUBLIC ${CASS_H})
target_link_libraries(cassandra_integration ${CASS_LIB})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
