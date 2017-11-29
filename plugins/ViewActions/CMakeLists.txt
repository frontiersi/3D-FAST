cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project( plugins_3dfast_ViewActions)

set(WITH_FACETOOLS TRUE)
include( "cmake/FindLibs.cmake")

set( BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories( "${INCLUDE_DIR}")
include_directories( "${PROJECT_BINARY_DIR}")

set( SOURCES
    "${SRC_DIR}/ViewActions.cpp"
    "${SRC_DIR}/ActionToggleShowBoundary.cpp"
    "${SRC_DIR}/ActionToggleShowLandmarks.cpp"
    )

set( QOBJECTS
    "${INCLUDE_DIR}/ViewActions.h"
    "${INCLUDE_DIR}/ActionToggleShowBoundary.h"
    "${INCLUDE_DIR}/ActionToggleShowLandmarks.h"
    )

qt5_wrap_cpp( QOBJECT_MOCS ${QOBJECTS})

add_library( ${PROJECT_NAME} ${SOURCES} ${QOBJECTS} ${QOBJECT_MOCS})
include( "cmake/LinkTargets.cmake")

install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION "lib")
install( FILES "${INCLUDE_DIR}/ViewActions.h" DESTINATION "include")



