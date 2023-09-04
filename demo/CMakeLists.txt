cmake_minimum_required(VERSION 3.8)

project(ivcpClient C CXX)

include(../cmake/common.cmake)

include_directories ("/usr/local/apr/include/apr-1")

set(CMAKE_BUILD_TYPE "Debug")

set(SUB_DIR_LIST  
                  ${CMAKE_CURRENT_SOURCE_DIR}/../libflv
                  ${CMAKE_CURRENT_SOURCE_DIR}/../libhls
                  ${CMAKE_CURRENT_SOURCE_DIR}/../libmpeg
                 )

foreach(SUB_DIR ${SUB_DIR_LIST})
include_directories(${SUB_DIR})
aux_source_directory(${SUB_DIR} SRC_LIST)
endforeach()

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SRC_LIST})

find_library(APR_LIBRARY apr-1 PATHS /usr/local/apr/lib/)
find_library(APR_UTIL_LIBRARY aprutil-1 PATHS /usr/local/apr/lib/)
find_library(MINIXML_LIBRARY mxml)
find_library(CURL_LIBRARY curl)
find_library(COS_C_SDK cos_c_sdk)


target_link_libraries(${PROJECT_NAME}  -livcpSdk 
  ${COS_C_SDK} ${APR_LIBRARY} ${APR_UTIL_LIBRARY} ${MINIXML_LIBRARY} ${CURL_LIBRARY}
  -ljsoncpp -luv -lglog librtmp.a crypto ssl -luuid -lpthread )


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./bin)



