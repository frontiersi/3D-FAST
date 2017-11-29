#!/usr/bin/env python
from CMakeBuild import CMakeBuilder
from CMakeBuild import makeDirectoryPath
from CMakeBuild import EnvDirs
import sys
import os


def getPluginDirs( appdir):
    """Get the plugin development directories."""
    pluginsdir = os.path.join( appdir, 'plugins')
    if not os.path.isdir(pluginsdir):
        print( "Can't find plugins development directory inside %s" % appdir)
        sys.exit(1)

    pdirs = [os.path.realpath( os.path.join( pluginsdir, p)) for p in os.listdir(pluginsdir)]
    for pdir in pdirs:
        if not os.path.exists( os.path.join( pdir, 'CMakeLists.txt')):
            print( "Didn't find CMakeLists.txt in '%s'!" % pdir)
            sys.exit(1)
    return pdirs


def buildPlugins( pluginDirs, appName):
    envdirs = EnvDirs()
    lbdir = envdirs.getBuildEnv()

    # Set the top level build directory (~/local_builds/3DFAST/plugins)
    lplugins = os.path.join( lbdir, appName, 'plugins')
    makeDirectoryPath(lplugins)

    # Create the debug and release installation directories    
    debugInstallDir = os.path.join( envdirs.getInstallEnv(), appName.lower(), 'Debug')
    releaseInstallDir = os.path.join( envdirs.getInstallEnv(), appName.lower(), 'Release')

    # Build the plugins (all release first, then all debug)
    cmakeDir = os.path.join( envdirs.getDevEnv(), 'libbuild', 'cmake') # Location of cmake template files for CMakeBuilder
    for pluginDir in pluginDirs:
        pname = pluginDir.split(os.path.sep)[-1]
        pluginBuildDir = os.path.join( lplugins, pname)

        # Build and install the release library
        mb = CMakeBuilder( cmakeDir, pluginDir, False, pluginBuildDir)
        mb.cmake()
        mb.build()
        mb.install( releaseInstallDir)
        print

        # Build and install the debug library
        mb = CMakeBuilder( cmakeDir, pluginDir, True, pluginBuildDir)
        mb.cmake()
        mb.build()
        mb.install( debugInstallDir)
        print


if __name__ == "__main__":
    envdirs = EnvDirs()
    if len(envdirs.getBuildEnv()) == 0 or len(envdirs.getInstallEnv()) == 0:
        sys.exit(1)

    appName = '3DFAST'
    appDir = os.path.join( 'D:\\' if sys.platform == 'win32' else os.environ['HOME'], 'dev', appName)
    if not os.path.exists( os.path.join( appDir, 'CMakeLists.txt')):
        print( "Didn't find CMakeLists.txt in '%s'!" % appDir)
        sys.exit(1);

    pluginDirs = getPluginDirs( appDir)
    buildPlugins( pluginDirs, appName)

    sys.exit(0)
