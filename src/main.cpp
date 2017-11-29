#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QtPlugin>
#include <sstream>
#include "FaceAppMain.h"

Q_IMPORT_PLUGIN( CoreFileIO)
Q_IMPORT_PLUGIN( CoreVisualisations)
Q_IMPORT_PLUGIN( ViewActions)
Q_IMPORT_PLUGIN( ToolActions)


int main( int argc, char* argv[])
{
    Q_INIT_RESOURCE(res);

    QApplication app( argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);

    std::ostringstream adeets;
    adeets << "Copyright 2017 " << APP_AUTHOR_NAME << " (" << APP_AUTHOR_EMAIL << ") ";
    QCoreApplication::setOrganizationName( adeets.str().c_str());

    std::ostringstream oss;
    oss << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_PATCH << "." << APP_BUILD_DTSTAMP;
    QCoreApplication::setApplicationVersion( oss.str().c_str());
    std::cerr
    << " --------------------------------------------------------------------------------------------\n"
    << " | Welcome to " << APP_DESCRIPTION << " version " << oss.str() << " |\n"
    << " | " << std::setw( 89) << adeets.str() << "|\n"
    << " --------------------------------------------------------------------------------------------\n"
    << "  Licensed under GNU General Public License v3.0 (https://www.gnu.org/licenses/gpl-3.0.html).\n"
    << "  3D-FAST's source code can be downloaded from https://github.com/richeytastic/3D-FAST.\n"
    << "  The developers gratefully acknowledge the authors of the following works used herein:\n"
    << "  - FaceTools (https://github.com/richeytastic/FaceTools)\n"
    << "  - rFeatures (https://github.com/richeytastic/rFeatures)\n"
    << "  - rModelIO (https://github.com/richeytastic/rModelIO)\n"
    << "  - libbuild (https://github.com/richeytastic/libbuild)\n"
    << "  - QTools (https://github.com/richeytastic/QTools)\n"
    << "  - rVTK (https://github.com/richeytastic/rVTK)\n"
    << "  - rlib (https://github.com/richeytastic/rlib)\n"
    << "  - Dlib (http://dlib.net/)\n"
    << "  - Qt (https://www.qt.io/)\n"
    << "  - VTK (https://www.vtk.org/)\n"
    << "  - OpenCV (https://opencv.org/)\n"
    << "  - CGAL (https://www.cgal.org/)\n"
    << "  - Boost v1.63 (http://www.boost.org/)\n"
    << "  - Eigen3 (http://eigen.tuxfamily.org/)\n"
    << "  - LibICP (https://github.com/symao/libicp/)\n"
    << "  - MiKTeX Portable (https://miktex.org/portable)\n"
    << "  - IDTF to U3D converter (http://www2.iaas.msu.ru/tmp/u3d/)\n"
    << "  - Open Asset Import Library v3.3.1 (http://assimp.sourceforge.net/)\n"
    << std::endl;

    std::cerr << "[[[ 3D-FAST debug output follows ]]]" << std::endl;

    QCommandLineParser parser;
    parser.setApplicationDescription(APP_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "The model file(s) to open."));
    parser.process(app);

    FaceApp::FaceAppMain* mainWin = new FaceApp::FaceAppMain;
    foreach ( const QString& fname, parser.positionalArguments())
        mainWin->loadModel(fname);

    mainWin->show();
    const int rval = app.exec();
    delete mainWin;
    return rval;
}   // end main
