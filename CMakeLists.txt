cmake_minimum_required(VERSION 3.3)
#SET(CMAKE_C_COMPILER 	/opt/compiler/gcc-8.2/bin/gcc )
#SET(CMAKE_CXX_COMPILER  /opt/compiler/gcc-8.2/bin/g++ )

project(test-cluster)

#sudo apt-get install libjsoncpp-dev 
#sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json
# sudo apt-get install libgflags-dev


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -fPIC -Wno-error=unused-variable")

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
 
set(PUBLIC /home/public/)

include_directories(
  ${PUBLIC}/jsoncpp/include
#  ${PUBLIC}/gflags/include
#  ${PUBLIC}/curl/include
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/butils/include
  ${CMAKE_CURRENT_SOURCE_DIR}/blog4cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/consul/include
  ${CMAKE_CURRENT_SOURCE_DIR}/consul/include/ppconsul
  /usr/include
  /usr/include/jsoncpp/
)

link_directories(
    ${PUBLIC}/jsoncpp/lib
 #   ${PUBLIC}/gflags/lib
 #   ${PUBLIC}/curl/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    /usr/lib 
    /usr/lib64
    /usr/local/lib
)

set(BOOST_ROOT  ${PUBLIC}/boost)
set(BOOST_MIN_VERSION 1.73) # Because of https://svn.boost.org/trac/boost/ticket/8759
set(USE_BOOST_REGEX OFF)

set(BOOST_COMPONENTS 
  date_time 
  chrono 
  filesystem 
  iostreams 
  program_options 
  regex 
  system 
  thread 
  unit_test_framework
)

find_package(Boost  COMPONENTS ${BOOST_COMPONENTS} REQUIRED )
if(Boost_FOUND)
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    link_directories(${Boost_LIBRARIES} )
    link_directories(${Boost_LIBRARY_DIRS} )
endif()

find_package( OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS} )
    link_directories(${OPENSSL_LIBRARIES} )
endif()

FIND_LIBRARY(RESOLV_LIB resolv /usr/lib /usr/local/lib NO_DEFAULT_PATH)

file(GLOB SRC_CLUSTER 
	./butils/include/base64.hpp
	./butils/src/base64.cpp
	./consul/src/*.cpp 
	./consul/src/ppconsul/*.cpp 
)
#
add_library(cluster_sdk ${SRC_CLUSTER})

set(SOURCE_FILES test.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries( ${CMAKE_PROJECT_NAME} 
	cluster_sdk
  	jsoncpp
	gflags
	curl
	${Boost_LIBRARIES}
	-lresolv 
	-lpthread
	-lrt
	-ldl
	-lz
	)

 


