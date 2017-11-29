cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project( plugins_3dfast_corevisualisation)

set(WITH_FACETOOLS TRUE)
include( "cmake/FindLibs.cmake")

set( BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")

include_directories( "${INCLUDE_DIR}")
include_directories( "${PROJECT_BINARY_DIR}")

set( SOURCES
    "${SRC_DIR}/CoreVisualisations.cpp"
    "${SRC_DIR}/TextureVisualisation.cpp"
    "${SRC_DIR}/SurfaceVisualisation.cpp"
    "${SRC_DIR}/WireframeVisualisation.cpp"
    "${SRC_DIR}/PointsVisualisation.cpp"
    )

set( QOBJECTS
    "${INCLUDE_DIR}/CoreVisualisations.h"
    "${INCLUDE_DIR}/TextureVisualisation.h"
    "${INCLUDE_DIR}/SurfaceVisualisation.h"
    "${INCLUDE_DIR}/WireframeVisualisation.h"
    "${INCLUDE_DIR}/PointsVisualisation.h"
    )

set( RESOURCE_FILE "iconsv.qrc")

qt5_wrap_cpp( QOBJECT_MOCS ${QOBJECTS})
qt5_add_resources( RESOURCE_FILE ${RESOURCE_FILE})

add_library( ${PROJECT_NAME} ${SOURCES} ${QOBJECTS} ${QOBJECT_MOCS} ${RESOURCE_FILE})
include( "cmake/LinkTargets.cmake")

install( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION "lib")
install( FILES "${INCLUDE_DIR}/CoreVisualisations.h" DESTINATION "include")

