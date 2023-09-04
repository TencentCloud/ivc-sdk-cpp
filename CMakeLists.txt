cmake_minimum_required(VERSION 3.8)

project(ivcpSdk C CXX)

include(./cmake/common.cmake)

set(ivcp_rpc_file_name "ivcp")
set (ivcpSdk_name "ivcpSdk")

# Proto file
get_filename_component(ivcp_proto "./protobuf/${ivcp_rpc_file_name}.proto" ABSOLUTE)
get_filename_component(ivcp_proto_path "${ivcp_proto}" PATH)

# Generated sources
set(rpc_out "${CMAKE_CURRENT_SOURCE_DIR}/clientPb")
set(ivcp_proto_srcs "${rpc_out}/${ivcp_rpc_file_name}.pb.cc")
set(ivcp_proto_hdrs "${rpc_out}/${ivcp_rpc_file_name}.pb.h")
set(ivcp_grpc_srcs "${rpc_out}/${ivcp_rpc_file_name}.grpc.pb.cc")
set(ivcp_grpc_hdrs "${rpc_out}/${ivcp_rpc_file_name}.grpc.pb.h")

add_custom_command(
      OUTPUT ${ivcp_proto_srcs} ${ivcp_proto_hdrs} ${ivcp_grpc_srcs} ${ivcp_grpc_hdrs}
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${rpc_out}"
        --cpp_out "${rpc_out}"
        -I "${ivcp_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${ivcp_proto}"
      DEPENDS "${ivcp_proto}")


add_library(ivcp_grpc_proto
  ${ivcp_grpc_srcs}
  ${ivcp_grpc_hdrs}
  ${ivcp_proto_srcs}
  ${ivcp_proto_hdrs})
target_link_libraries(ivcp_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

option(TEST_OPTION "TEST_OPTION"  OFF)


if (TEST_OPTION) 
add_definitions(-DUTEST)
endif()

set(SUB_DIR_LIST  ${CMAKE_CURRENT_SOURCE_DIR}/clientPb 
                  ${CMAKE_CURRENT_SOURCE_DIR}/interface
                  ${CMAKE_CURRENT_SOURCE_DIR}/interfaceImp 
                  ${CMAKE_CURRENT_SOURCE_DIR}/data
                  ${CMAKE_CURRENT_SOURCE_DIR}/grpcImp 
                  ${CMAKE_CURRENT_SOURCE_DIR}/rtmp
                  ${CMAKE_CURRENT_SOURCE_DIR}/cos 
                  ${CMAKE_CURRENT_SOURCE_DIR}/libflv
                  ${CMAKE_CURRENT_SOURCE_DIR}/libhls
                  ${CMAKE_CURRENT_SOURCE_DIR}/libmpeg
                  ${CMAKE_CURRENT_SOURCE_DIR}/stun 
                  ${CMAKE_CURRENT_SOURCE_DIR}/interface
                  ${CMAKE_CURRENT_SOURCE_DIR}/internalDef 
                  ${CMAKE_CURRENT_SOURCE_DIR}/p2p )

foreach(SUB_DIR ${SUB_DIR_LIST})
include_directories(${SUB_DIR})
aux_source_directory(${SUB_DIR} SRC_LIST)
endforeach()

include_directories ("/usr/local/apr/include/apr-1")

add_library(${ivcpSdk_name} SHARED ${SRC_LIST})
target_link_libraries(${ivcpSdk_name} 
  ivcp_grpc_proto
  absl::flags
  absl::flags_parse
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF} )


install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/interface/ DESTINATION  /usr/local/include)
INSTALL(TARGETS ${ivcpSdk_name} ivcp_grpc_proto LIBRARY DESTINATION /usr/local/lib ARCHIVE DESTINATION /usr/local/lib)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/./lib)