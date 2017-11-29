# - this module looks for Matlab
# Defines:
#  MATLAB_INCLUDE_DIR: include path for mex.h, engine.h
#  MATLAB_LIBRARIES:   required libraries: libmex, etc
#  MATLAB_MEX_LIBRARY: path to libmex.lib
#  MATLAB_MX_LIBRARY:  path to libmx.lib
#  MATLAB_ENG_LIBRARY: path to libeng.lib

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

message( STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

set(MATLAB_FOUND 0)
if(WIN32)
    set( MATLAB_REG_ENTRY "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\8.6;MATLABROOT]")
    get_filename_component( MATLAB_INSTALL_PATH ${MATLAB_REG_ENTRY} ABSOLUTE CACHE)
    set(MATLAB_ROOT "${MATLAB_INSTALL_PATH}/extern/lib/win64/microsoft")
    
    find_library(MATLAB_MAT_LIBRARY libmat ${MATLAB_ROOT})
    find_library(MATLAB_MX_LIBRARY libmx ${MATLAB_ROOT})

    find_path(MATLAB_INCLUDE_DIR "mat.h" "${MATLAB_INSTALL_PATH}/extern/include")
elseif(UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(MATLAB_ROOT "${MATLAB_DIR}/bin/glnx86")
    else()
        set(MATLAB_ROOT "${MATLAB_DIR}/bin/glnxa64")
    endif()

    find_library(MATLAB_MX_LIBRARY mx ${MATLAB_ROOT})
    find_library(MATLAB_MAT_LIBRARY mat ${MATLAB_ROOT})

    find_path(MATLAB_INCLUDE_DIR
        "mex.h"
        "${MATLAB_DIR}/extern/include/"
        "/usr/local/matlab-7sp1/extern/include/"
        "/opt/matlab-7sp1/extern/include/"
        "$ENV{HOME}/matlab-7sp1/extern/include/"
        "$ENV{HOME}/redhat-matlab/extern/include/"
        )
endif()

message( STATUS "MATLAB_ROOT:           ${MATLAB_ROOT}")
message( STATUS "MATLAB_MX_LIBRARY:     ${MATLAB_MX_LIBRARY}")
message( STATUS "MATLAB_MAT_LIBRARY:    ${MATLAB_MAT_LIBRARY}")

# Common to UNIX and Win32:
set(MATLAB_LIBRARIES
    ${MATLAB_MX_LIBRARY}
    ${MATLAB_MAT_LIBRARY}
    )

if(MATLAB_INCLUDE_DIR AND MATLAB_LIBRARIES)
    set(MATLAB_FOUND 1)
endif()

mark_as_advanced(
    MATLAB_LIBRARIES
    MATLAB_MX_LIBRARY
    MATLAB_MAT_LIBRARY
    MATLAB_INCLUDE_DIR
    MATLAB_FOUND
    MATLAB_ROOT
    MATLAB_INSTALL_PATH
    )

