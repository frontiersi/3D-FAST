# 3D-FAST (Facial Analysis for Clinical Translation)

## Prerequisites
- [libbuild](../../../libbuild)
- [FaceTools](../../../FaceTools)
- [QTools](../../../QTools)
- [rVTK](../../../rVTK)
- [rModelIO](../../../rModelIO)
- [rFeatures](../../../rFeatures)
- [rlib](../../../rlib)

3D-FAST is primarily for researchers and clinicians to assist them in the visualisation
and analysis of 3D facial images. It is currently in an early stage of development, and
feedback is being accepted to make modifications / incorporate desirable features.

If you would like to be involved in contributing, please contact the developers.

## Build instructions
Once all of the prerequisite libraries are installed, 3D-FAST's plugin libraries should be
built by running script makePlugins.py. Afterwards, create build directory for 3D-FAST
within which you should use CMake to bootstrap and build the tool. 3D-FAST can be
run directly from the bin directory and does not need to be installed.
