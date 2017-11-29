#include <CoreFileIO.h>
#include <XMLFileIO.h>
#include <OBJFileIO.h>
#include <AssImpIO.h>       // FaceTools
#include <boost/algorithm/string.hpp>
using FaceApp::CoreFileIO;
using FaceTools::FileIOInterface;


// public
CoreFileIO::CoreFileIO() : FaceTools::FaceActionGroup()
{
    addAction( new FaceApp::XMLFileIO);

    // Get all of AssImp supported filetypes
    RModelIO::AssetImporter importer;
    const boost::unordered_map<std::string, std::string>& iformats = importer.getAvailable();
    typedef std::pair<std::string, std::string> SPair;
    foreach ( const SPair& extDesc, iformats)
    {
        std::string ext = boost::algorithm::to_lower_copy(extDesc.first);
        if ( ext != "obj")
            addAction( new FaceTools::AssImpIO( ext));
    }   // end foreach

    addAction( new FaceApp::OBJFileIO);
}   // end ctor


