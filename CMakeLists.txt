cmake_minimum_required(VERSION 3.18)

project(BFXPlugin)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

add_subdirectory(BFX)

# TODO: make sure matches your local Maya path
set(MAYA_LOCATION "C:/Program Files/Autodesk/Maya2022")

if (NOT EXISTS "${MAYA_LOCATION}")
  message(FATAL_ERROR "Maya not found at ${MAYA_LOCATION}, please set MAYA_LOCATION to the correct path!!!")
endif()

# set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)
# include_directories(${PROJECT_INCLUDE_DIR})

# Add source files to the project
set(BFX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/BFX/src)

set(SOURCES
    ${BFX_SOURCE_DIR}/clipper.cpp
    src/NodeCmd.cpp
    src/PluginMain.cpp
    ${BFX_SOURCE_DIR}/vec.cpp
    src/NodePlacer.cpp
    src/NodePlacerNode.cpp
)

# Add header files to the project
set(HEADERS
    ${BFX_SOURCE_DIR}/clipper.h
    src/NodeCmd.h
    ${BFX_SOURCE_DIR}/vec.h
    src/NodePlacer.h
    src/NodePlacerNode.h
)

find_package(Maya REQUIRED)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    BFX
    Maya::Maya
)
target_include_directories(${PROJECT_NAME} 
    PUBLIC ${BFX_SOURCE_DIR}
    PRIVATE Maya::Maya
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
)
MAYA_PLUGIN(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} ${MAYA_TARGET_TYPE} DESTINATION plug-ins/${MAYA_VERSION})