cmake_minimum_required(VERSION 3.5)

project(shipcolor_pi)

set(PACKAGE_NAME "shipcolor_pi")
set(PACKAGE_VERSION "1.0.0")
set(CMAKE_CXX_STANDARD 11)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

find_package(OpenCPN REQUIRED)

include_directories(
    ${OPENCPN_INCLUDE_DIR}
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
    ${OPENCPN_LIBRARIES}
)

set_target_properties(${PACKAGE_NAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${PACKAGE_NAME}"
)
