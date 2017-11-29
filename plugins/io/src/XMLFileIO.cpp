#include <XMLFileIO.h>
#include <FaceModel.h>      // FaceTools
#include <FacesCohort.h>    // FaceTools
#include <MiscFunctions.h>  // FaceTools
#include <AssetImporter.h>  // RModelIO
#include <OBJExporter.h>    // RModelIO
using FaceApp::XMLFileIO;
using FaceTools::FaceModel;
typedef RFeatures::CameraParams Camera;
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <ctime>

XMLFileIO::XMLFileIO() : FaceTools::FileIOInterface()
{
    init();
}   // end ctor


// public
size_t XMLFileIO::getFileExtensions( std::vector<std::string>& exts) const
{
    exts.push_back(FaceApp::XML_FILE_EXTENSION);
    return 1;
}   // end getFileExtensions


// protected virtual
FaceModel* XMLFileIO::load( const std::string& fname)
{
    _err = "";
    FaceModel* fm = NULL;
    FaceTools::ObjMetaData::Ptr omd;

    try
    {
        FaceTools::FacesCohort faces( FaceApp::XML_VERSION);
        std::fstream ifs( fname.c_str());
        ifs >> faces;
        ifs.close();
        if ( faces.size() != 1)
        {
            _err = "XMLFileIO::load: Failed to load from " + fname;
            std::cerr << "[ERROR] " << _err << std::endl;
            return NULL;
        }   // end if

        omd = faces.getRecord(0);
        const std::string fext = FaceTools::getExtension( omd->getObjectFile());
        assert( fext == "obj");

        RModelIO::AssetImporter importer(true,true);
        importer.enableFormat(fext);
        const std::string modelfile = (boost::filesystem::path(fname).parent_path() / omd->getObjectFile()).string();
        std::cerr << "Got model file " << modelfile << std::endl;
        const RFeatures::ObjModel::Ptr model = importer.load( modelfile);   // Doesn't merge materials or clean!
        if ( !model)
        {
            _err = "XMLFileIO::load: Failed to load object from " + omd->getObjectFile();
            std::cerr << "[ERROR] " << _err << std::endl;
            return NULL;
        }   // end if
        omd->setObject( model);
    }   // end try
    catch ( const std::exception& e)
    {
        _err = e.what();
    }   // end catch

    if ( omd)
        fm = new FaceModel( omd);

    return fm;
}   // end load


// protected virtual
bool XMLFileIO::save( const FaceModel* fm, const std::string& fname)
{
    _err = "";
    if ( !fm)
    {
        _err = "NULL FaceModel on save!";
        return false;
    }   // end if

    try
    {
        const std::string fmt = "obj";
        RModelIO::OBJExporter exporter;
        const std::string modfile = boost::filesystem::path( fname).replace_extension( fmt).string();

        // Write out the model geometry itself into .obj format.
        RFeatures::ObjModel::Ptr model = fm->getObjectMeta()->getObject();
        std::cerr << "[STATUS] FaceApp::XMLFileIO::save: Exporting model to " << modfile << std::endl;
        if ( !exporter.save( model, modfile))
        {
            _err = exporter.err();
            std::cerr << "[WARNING] FaceApp::XMLFileIO::save: Failed! " << _err << std::endl;
        }   // end if
        else
        {
            std::cerr << "[STATUS] FaceApp::XMLFileIO::save: Successfully exported model to " << modfile << std::endl;
            std::cerr << "[STATUS] FaceApp::XMLFileIO::save: Writing meta-data out to " << fname << std::endl;
            FaceTools::ObjMetaData::Ptr omd = fm->getObjectMeta()->copy( false);        // Don't build a new KD tree
            const std::string relativeModelFile = boost::filesystem::path(modfile).filename().string();
            omd->setObjectFile( relativeModelFile);;  // Don't include the path

            FaceTools::FacesCohort faces( FaceApp::XML_VERSION);
            std::ostringstream desc;
            desc << FaceApp::XML_FILE_DESCRIPTION << ";SINGLE;" << time(NULL);
            faces.setDescription( desc.str());
            faces.addRecord( omd);

            std::ofstream ofs( fname.c_str());
            ofs << faces << std::endl;
            ofs.close();
        }   // end else
    }   // end try
    catch ( const std::exception& e)
    {
        std::cerr << "[EXCEPTION!] FaceApp::XMLFileIO::save: Failed to write to " << fname << std::endl;
        _err = e.what();
    }   // end catch

    return _err.empty();
}   // end save
