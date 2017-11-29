cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project( plugins_3dfast_corefileio)

set(WITH_FACETOOLS TRUE)
include( "cmake/FindLibs.cmake")

set( BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories( "${INCLUDE_DIR}")
include_directories( "${PROJECT_BINARY_DIR}")

set( SOURCES "${SRC_DIR}/CoreFileIO.cpp"
             "${SRC_DIR}/OBJFileIO.cpp"
             "${SRC_DIR}/XMLFileIO.cpp"
             #"${SRC_DIR}/MatlabFileIO.cpp"
             )

set( QOBJECTS "${INCLUDE_DIR}/CoreFileIO.h"
              "${INCLUDE_DIR}/OBJFileIO.h"
              "${INCLUDE_DIR}/XMLFileIO.h"
              #"${INCLUDE_DIR}/MatlabFileIO.h"
              )

qt5_wrap_cpp( QOBJECT_MOCS ${QOBJECTS})

add_library( ${PROJECT_NAME} ${SOURCES} ${QOBJECTS} ${QOBJECT_MOCS})
include( "cmake/LinkTargets.cmake")

install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION "lib")
install( FILES "${INCLUDE_DIR}/CoreFileIO.h" DESTINATION "include")
