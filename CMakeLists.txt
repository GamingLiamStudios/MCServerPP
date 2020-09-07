cmake_minimum_required(VERSION 3.0)
project(MCServer++)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /arch:AVX2 /Za /Oxi")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Ofast -march=native")
endif()

include_directories(${PROJECT_NAME}/Headers/
                    ${PROJECT_NAME}/Include/)
file(GLOB PROJECT_HEADERS ${PROJECT_NAME}/Headers/*.hpp)
file(GLOB PROJECT_SOURCES ${PROJECT_NAME}/Sources/*.cpp)
file(GLOB PROJECT_CONFIGS CMakeLists.txt)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})

add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}) 

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)         
ENDIF()      
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
