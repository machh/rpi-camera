cmake_minimum_required(VERSION 3.3)
#SET(CMAKE_C_COMPILER 	/opt/compiler/gcc-8.2/bin/gcc )
#SET(CMAKE_CXX_COMPILER  /opt/compiler/gcc-8.2/bin/g++ )

project(rpi-camera)

if(CMAKE_VERSION VERSION_LESS "3.1.3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fPIC -Wno-error=unused-variable")

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
 
set(PUBLIC /home/public/)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdpart
  ${CMAKE_CURRENT_SOURCE_DIR}/sensor
  /usr/include
  /usr/include/jsoncpp/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    /usr/lib 
    /usr/lib64
    /usr/local/lib
)

FILE(GLOB SOURCE_FILES
    "*/*.h"
    "*/*.cpp"  
    "*/*/*.cpp"  
    "*/*/*.h"
    "*/*/*.cc"  
    "*.h"  
    "*.cpp"
)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} 
	-lpthread
	-lrt
	-ldl
	-lz
	)

 


