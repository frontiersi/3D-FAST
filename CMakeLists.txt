cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

PROJECT( 3dfast)

# Version code: A.B.C.D
# A: APP_VERSION_MAJOR - Significant feature additions or major changes to how the user works with the tool.
# B: APP_VERSION_MINOR - Some feature additions or significant changes to how existing features are implemented.
# C: APP_VERSION_PATCH - Bug fixes and/or minor changes to how existing features are implemented.
# D: APP_BUILD_DTSTAMP - Date-time stamp of build (coordinated universal time).
set( APP_VERSION_MAJOR 2)
set( APP_VERSION_MINOR 0)
set( APP_VERSION_PATCH 1)
string( TIMESTAMP APP_BUILD_DTSTAMP "%y%m%d%H%M%S" UTC)

set( APP_AUTHOR_NAME "\"Richard Palmer & Stefanie Kung\"")
set( APP_AUTHOR_EMAIL "\"r.l.palmer@curtin.edu.au\"")

set( APP_NAME "\"3D-FAST\"")
set( APP_DESCRIPTION "\"3D-FAST (Facial Analysis for Clinical Translation)\"")

set( APP_OPTION_PREFS "\"prefs.etc\"")
file( COPY "prefs.etc" DESTINATION "${PROJECT_BINARY_DIR}/bin") # Copy over the preferences file

# Copy across the face detection models to the build binary directory
set( FACE_MODELS_DIR "facemodels")
set( HAAR_CASCADES_MODELS "\"${FACE_MODELS_DIR}/haarcascades\"")
set( FACE_SHAPE_LANDMARKS "\"${FACE_MODELS_DIR}/shape_predictor_68_face_landmarks.dat\"")
file( COPY "${PROJECT_SOURCE_DIR}/${FACE_MODELS_DIR}" DESTINATION "${PROJECT_BINARY_DIR}/bin")

# Copy across the IDTF converter and pdflatex (WIN32)
if(WIN32)
    file( COPY "${PROJECT_SOURCE_DIR}/u3dIntelWin" DESTINATION "${PROJECT_BINARY_DIR}/bin")
    set( IDTF_CONVERTER "\"u3dIntelWin/IDTFConverter.exe\"")
    #file( COPY "${PROJECT_SOURCE_DIR}/texmfs" DESTINATION "${PROJECT_BINARY_DIR}/bin")  # MiKTeX portable
    set( PDF_LATEX "\"texmfs/install/miktex/bin/pdflatex.exe\"")
else()
    file( COPY "${PROJECT_SOURCE_DIR}/u3dIntel" DESTINATION "${PROJECT_BINARY_DIR}/bin")
    set( IDTF_CONVERTER "\"u3dIntel/bin/IDTFConverter\"")
    set( PDF_LATEX "\"pdflatex\"")
endif()

# Create the working directory for PDF creation and copy over the logo.
set( _LaTeXWorkDir "${PROJECT_BINARY_DIR}/bin/texwork")
file( MAKE_DIRECTORY "${_LaTeXWorkDir}")
file( COPY "res/logoTeX.png" DESTINATION "${_LaTeXWorkDir}")
set( LATEX_WORK "\"texwork\"")
set( LATEX_LOGO "\"texwork/logoTeX.png\"")

#include("cmake/Packaging.cmake")

set(WITH_FACETOOLS TRUE)
file( TO_CMAKE_PATH $ENV{DEV_PARENT_DIR} DEV_PARENT_DIR)
include( "${DEV_PARENT_DIR}/libbuild/cmake/FindLibs.cmake")

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set( FORMS_DIR "${PROJECT_SOURCE_DIR}/forms")

configure_file( "${INCLUDE_DIR}/${PROJECT_NAME}_Config.h.in"
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_Config.h")

set( FORMS
    "${FORMS_DIR}/AboutDialog.ui"
    "${FORMS_DIR}/FaceAppMain.ui"
    "${FORMS_DIR}/HelpDialog.ui"
    "${FORMS_DIR}/LandmarksDialog.ui"
    "${FORMS_DIR}/LicensesDialog.ui"
    "${FORMS_DIR}/ModelInfoDialog.ui"
    "${FORMS_DIR}/OptionsDialog.ui"
    "${FORMS_DIR}/SplashWindow.ui"
    )

set( QOBJECTS
    "${INCLUDE_DIR}/ActionExportPDF.h"
    "${INCLUDE_DIR}/AboutDialog.h"
    "${INCLUDE_DIR}/CoreDialogs.h"
    "${INCLUDE_DIR}/FaceAppMain.h"
    "${INCLUDE_DIR}/FaceModelManager.h"
    "${INCLUDE_DIR}/HelpDialog.h"
    "${INCLUDE_DIR}/LandmarksDialog.h"
    "${INCLUDE_DIR}/LicensesDialog.h"
    "${INCLUDE_DIR}/ModelInfoDialog.h"
    "${INCLUDE_DIR}/MultiFaceModelViewer.h"
    "${INCLUDE_DIR}/OptionsDialog.h"
    "${INCLUDE_DIR}/SplashWindow.h"
    )

set( INCLUDE_FILES
    ${QOBJECTS}
    )

set( SRC_FILES
    ${SRC_DIR}/main
    ${SRC_DIR}/AboutDialog
    ${SRC_DIR}/ActionExportPDF
    ${SRC_DIR}/CoreDialogs
    ${SRC_DIR}/FaceAppMain
    ${SRC_DIR}/FaceModelManager
    ${SRC_DIR}/HelpDialog
    ${SRC_DIR}/LandmarksDialog
    ${SRC_DIR}/LicensesDialog
    ${SRC_DIR}/ModelInfoDialog
    ${SRC_DIR}/MultiFaceModelViewer
    ${SRC_DIR}/OptionsDialog
    ${SRC_DIR}/SplashWindow
    )

set( RESOURCE_FILE "res.qrc")
qt5_wrap_ui( FORM_HEADERS ${FORMS})
qt5_wrap_cpp( QOBJECT_MOCS ${QOBJECTS})
qt5_add_resources( RESOURCE_FILE ${RESOURCE_FILE})

# The root directory of 3D-FAST's separately compiled libraries.
# Archives are in the lib folder, shared objects in the bin folder.
file( TO_CMAKE_PATH $ENV{INSTALL_PARENT_DIR} INSTALL_PARENT_DIR)
set( APP_LIBRARY_ROOT "${INSTALL_PARENT_DIR}/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}")

# Add the libraries for static linking which should already be built.
set( STATIC_LIBRARY_DIR "${APP_LIBRARY_ROOT}/lib")  # Location of statically linked libraries
set( STATIC_LIBRARY_INCLUDE_DIR "${APP_LIBRARY_ROOT}/include")  # Headers for statically linked libraries
add_library( ViewActions STATIC IMPORTED)
add_library( ToolActions STATIC IMPORTED)
add_library( CoreVisualisations STATIC IMPORTED)
add_library( CoreFileIO STATIC IMPORTED)
set( _lib_suff "a")
set( _lib_prfx "lib")
if(WIN32)
    set( _lib_suff "lib")
    set( _lib_prfx "")
endif()
set_property( TARGET ViewActions        PROPERTY IMPORTED_LOCATION "${STATIC_LIBRARY_DIR}/${_lib_prfx}plugins_3dfast_ViewActions${_dsuffix}.${_lib_suff}")
set_property( TARGET ToolActions        PROPERTY IMPORTED_LOCATION "${STATIC_LIBRARY_DIR}/${_lib_prfx}plugins_3dfast_ToolActions${_dsuffix}.${_lib_suff}")
set_property( TARGET CoreVisualisations PROPERTY IMPORTED_LOCATION "${STATIC_LIBRARY_DIR}/${_lib_prfx}plugins_3dfast_corevisualisation${_dsuffix}.${_lib_suff}")
set_property( TARGET CoreFileIO         PROPERTY IMPORTED_LOCATION "${STATIC_LIBRARY_DIR}/${_lib_prfx}plugins_3dfast_corefileio${_dsuffix}.${_lib_suff}")

# Resource file for exe icon (used by Windows)
set( ICON_RC "icon.rc")

include_directories( ${INCLUDE_DIR})
include_directories( ${PROJECT_BINARY_DIR})
include_directories( ${STATIC_LIBRARY_INCLUDE_DIR})

# Add WIN32 to prevent console window from opening
add_executable( ${PROJECT_NAME}
    ${SRC_FILES}
    ${INCLUDE_FILES}
    ${FORMS}
    ${FORM_HEADERS}
    ${QOBJECT_MOCS}
    ${ICON_RC}
    ${RESOURCE_FILE}
    )

target_link_libraries( ${PROJECT_NAME} ViewActions)
target_link_libraries( ${PROJECT_NAME} ToolActions)
target_link_libraries( ${PROJECT_NAME} CoreVisualisations)
target_link_libraries( ${PROJECT_NAME} CoreFileIO)

set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# On Windows, there's no RPATH so we must collect all the DLLs and place into the project binary dir.
include( "${DEV_PARENT_DIR}/libbuild/cmake/ExeInstall.cmake")

# Copy over dynamically loaded plugins to bin/plugins directory
set( _pluginsDir "${PROJECT_BINARY_DIR}/bin/plugins")
file( MAKE_DIRECTORY "${_pluginsDir}")
file( GLOB _plugins "${LIB_PRE_REQS}/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}/bin/*")
file( COPY ${_plugins} DESTINATION ${_pluginsDir})
