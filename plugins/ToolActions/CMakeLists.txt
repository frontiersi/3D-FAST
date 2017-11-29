cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project( plugins_3dfast_ToolActions)

set(WITH_FACETOOLS TRUE)
include( "cmake/FindLibs.cmake")

set( BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories( "${INCLUDE_DIR}")
include_directories( "${PROJECT_BINARY_DIR}")

set( QOBJECTS
    "${INCLUDE_DIR}/ActionAddLandmark.h"
    "${INCLUDE_DIR}/ActionCropFace.h"
    "${INCLUDE_DIR}/ActionDeleteLandmark.h"
    "${INCLUDE_DIR}/ActionDetectFace.h"
    "${INCLUDE_DIR}/ActionMoveLandmarks.h"
    "${INCLUDE_DIR}/ActionToggleCalipers.h"
    "${INCLUDE_DIR}/ToolActions.h"
    )

set( SOURCES
    "${SRC_DIR}/ActionAddLandmark.cpp"
    "${SRC_DIR}/ActionCropFace.cpp"
    "${SRC_DIR}/ActionDeleteLandmark.cpp"
    "${SRC_DIR}/ActionDetectFace.cpp"
    "${SRC_DIR}/ActionMoveLandmarks.cpp"
    "${SRC_DIR}/ActionToggleCalipers.cpp"
    "${SRC_DIR}/ToolActions.cpp"
    )

qt5_wrap_cpp( QOBJECT_MOCS ${QOBJECTS})

add_library( ${PROJECT_NAME} ${SOURCES} ${QOBJECTS} ${QOBJECT_MOCS})
include( "cmake/LinkTargets.cmake")

install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION "lib")
install( FILES "${INCLUDE_DIR}/ToolActions.h" DESTINATION "include")

