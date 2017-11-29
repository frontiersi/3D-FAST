#include <OBJFileIO.h>
#include <FaceModel.h>      // FaceTools
#include <AssetImporter.h>  // RModelIO
using FaceApp::OBJFileIO;
using FaceTools::FaceModel;


OBJFileIO::OBJFileIO() : FaceTools::FileIOInterface()
{
    init();
}   // end ctor


// protected virtual
FaceModel* OBJFileIO::load( const std::string& fname)
{
    _err = "";
    RModelIO::AssetImporter importer(true,true);
    importer.enableFormat("obj");
    const RFeatures::ObjModel::Ptr model = importer.load( fname);
    if ( !model)
    {
        _err = "OBJFileIO::load: Failed to load object from " + fname;
        std::cerr << "[ERROR] " << _err << std::endl;
        return NULL;
    }   // end if

    FaceTools::ObjMetaData::Ptr omd = FaceTools::ObjMetaData::create( fname, model);
    return new FaceModel( omd);
}   // end load


// protected virtual
bool OBJFileIO::save( const FaceModel* fm, const std::string& fname)
{
    _err = "";
    if ( !fm)
    {
        _err = "NULL FaceModel on save!";
        return false;
    }   // end if

    RFeatures::ObjModel::Ptr model = fm->getObjectMeta()->getObject();
    std::cerr << "[STATUS] FaceApp::OBJFileIO::save: Exporting model to " << fname << std::endl;
    if ( _exporter.save( model, fname))
        std::cerr << "[STATUS] FaceApp::OBJFileIO::save: Successfully exported model to " << fname << std::endl;
    else
    {
        _err = _exporter.err();
        std::cerr << "[WARNING] FaceApp::OBJFileIO::save: Failed! " << _err << std::endl;
    }   // end else
    return _err.empty();
}   // end save


size_t OBJFileIO::getFileExtensions( std::vector<std::string>& exts) const
{
    exts = _exporter.getExtensions();
    return exts.size();
}   // end getFileExtensions


std::string OBJFileIO::getFileDescription() const
{
    return _exporter.getDescription( _exporter.getExtensions()[0]);
}   // end getFileDescription
