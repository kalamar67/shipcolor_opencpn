cmake_minimum_required(VERSION 3.5)
project(shipcolor_pi)

set(PACKAGE_NAME "shipcolor_pi")
set(CMAKE_CXX_STANDARD 11)

if(WIN32)
    add_definitions(-D__WXMSW__)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}/opencpn_libs/api-18
    ${wxWidgets_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

set(SRC
    src/shipcolor_pi.cpp
    src/shipcolor_pi.h
)

add_library(${PACKAGE_NAME} SHARED ${SRC})

target_link_libraries(${PACKAGE_NAME}
    ${wxWidgets_LIBRARIES}
)

set_target_properties(${PACKAGE_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${PACKAGE_NAME}"
)
